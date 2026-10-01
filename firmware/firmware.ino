
#include "display_driver.h"
#include "updaters.h"
#include "mutex.h"

static lv_disp_draw_buf_t draw_buf;
#ifndef WAVESHARE_S3_LCD7B
static lv_color_t disp_draw_buf1[LCD_WIDTH * LCD_HEIGHT / 10];
static lv_color_t disp_draw_buf2[LCD_WIDTH * LCD_HEIGHT / 10];
#endif
static lv_disp_drv_t disp_drv;

#ifdef PERF_MONITOR
// print refresh rate and render time once per second
static void perf_monitor_cb(lv_disp_drv_t *drv, uint32_t time, uint32_t px) {
  static uint32_t frames = 0, busy = 0, pixels = 0, last = 0;
  frames++;
  busy += time;
  pixels += px;
  uint32_t now = millis();
  if (now - last >= 1000) {
    Serial.printf("PERF fps:%" PRIu32 " render:%" PRIu32 "ms/s flush:%" PRIu32 "ms/s px:%" PRIu32 "\n", frames, busy, flushTimeUs / 1000, pixels);
    flushTimeUs = 0;
    frames = busy = pixels = 0;
    last = now;
  }
}
#endif

void setup(void) {

#if defined(DEBUG) || defined(PERF_MONITOR)
  Serial.begin(115200);
  Serial.println("Setup starting");
#endif

  mutex_init();
  lcd_panel_start();
  lv_init();

  // disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * LCD_HEIGHT / 10, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  // disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * LCD_HEIGHT / 10, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#ifdef WAVESHARE_S3_LCD7B
  // too big for static internal RAM at 1024x600, flush is synchronous so one buffer is enough
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * LCD_DRAW_BUF_LINES, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LCD_WIDTH * LCD_DRAW_BUF_LINES);
#else
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, LCD_WIDTH * LCD_HEIGHT / 10);
#endif

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = disp_flush_callback;
  disp_drv.draw_buf = &draw_buf;
#ifdef PERF_MONITOR
  disp_drv.monitor_cb = perf_monitor_cb;
#endif
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = &touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // Crate task get data and updater
  xTaskCreatePinnedToCore(TaskCANReceiver, "TaskCANReceiver", 4 * 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskHeartbeat, "TaskHeartbeat", 2 * 1024, NULL, 1, NULL, 0);

  ui_init();

#ifdef DEBUG
  Serial.println("Setup done");
#endif
}

void loop(void) {
  if (xSemaphoreTake(uiMutex, portMAX_DELAY) == pdTRUE) {
    lv_timer_handler();
    xSemaphoreGive(uiMutex);
  }

  vTaskDelay(pdMS_TO_TICKS(5));
}