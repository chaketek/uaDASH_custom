#include "display_driver.h"

#ifdef WAVESHARE_S3_LCD7B

// Waveshare ESP32-S3-Touch-LCD-7B: esp_lcd RGB panel with bounce buffers.
// LVGL renders into internal RAM and the flush copies into the PSRAM
// framebuffer (rendering directly into PSRAM starves the scanout, async copy
// by another core or GDMA turned out slower).
// The panel is created on core 0 so its bounce buffer interrupt does not take
// time from LVGL running on core 1.

#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_ops.h>

int brightnessVal = 205;
#ifdef PERF_MONITOR
uint32_t flushTimeUs = 0;
#endif

static esp_lcd_panel_handle_t panel = NULL;
static void *framebuffer = NULL;
static lgfx::Touch_GT911 touch;
static uint8_t expanderOut = 0xFF;

static SemaphoreHandle_t panelReady;

static void expanderWrite(uint8_t reg, uint8_t val) {
  uint8_t buf[2] = { reg, val };
  auto res = lgfx::i2c::transactionWrite(EXPANDER_I2C_PORT, EXPANDER_I2C_ADDR, buf, 2, TOUCH_FREQ);
#ifdef DEBUG
  Serial.printf("expander reg 0x%02X <- 0x%02X : %s\n", reg, val, res.has_value() ? "ok" : "fail");
#endif
}

static void expanderDigitalWrite(uint8_t pin, bool level) {
  if (level) {
    expanderOut |= (1 << pin);
  } else {
    expanderOut &= ~(1 << pin);
  }
  expanderWrite(EXPANDER_REG_OUTPUT, expanderOut);
}

// expander PWM is inverted (0: full bright, 255: off)
static void writeBacklight(int val) {
  expanderWrite(EXPANDER_REG_PWM, (uint8_t)(255 - val));
}

static void panelInit() {
  esp_lcd_rgb_panel_config_t cfg;
  memset(&cfg, 0, sizeof(cfg));
  cfg.clk_src = LCD_CLK_SRC_DEFAULT;
  cfg.timings.pclk_hz = LCD_FREQ;
  cfg.timings.h_res = LCD_WIDTH;
  cfg.timings.v_res = LCD_HEIGHT;
  cfg.timings.hsync_pulse_width = LCD_HSYNC_PULSE_WIDTH;
  cfg.timings.hsync_back_porch = LCD_HSYNC_BACK_PORCH;
  cfg.timings.hsync_front_porch = LCD_HSYNC_FRONT_PORCH;
  cfg.timings.vsync_pulse_width = LCD_VSYNC_PULSE_WIDTH;
  cfg.timings.vsync_back_porch = LCD_VSYNC_BACK_PORCH;
  cfg.timings.vsync_front_porch = LCD_VSYNC_FRONT_PORCH;
  cfg.timings.flags.pclk_active_neg = LCD_PCLK_ACTIVE_NEG;
  cfg.timings.flags.de_idle_high = LCD_DE_IDLE_HIGH;
  cfg.timings.flags.pclk_idle_high = LCD_PCLK_IDLE_HIGH;
  cfg.data_width = 16;
  cfg.bits_per_pixel = 16;
  cfg.num_fbs = 1;
  cfg.bounce_buffer_size_px = LCD_WIDTH * LCD_BOUNCE_LINES;
  cfg.dma_burst_size = 64;
  cfg.hsync_gpio_num = LCD_PIN_HSYNC;
  cfg.vsync_gpio_num = LCD_PIN_VSYNC;
  cfg.de_gpio_num = LCD_PIN_HENABLE;
  cfg.pclk_gpio_num = LCD_PIN_PCLK;
  cfg.disp_gpio_num = GPIO_NUM_NC;

  const int8_t data_pins[16] = {
    LCD_PIN_DATA_B0, LCD_PIN_DATA_B1, LCD_PIN_DATA_B2, LCD_PIN_DATA_B3, LCD_PIN_DATA_B4,
    LCD_PIN_DATA_G0, LCD_PIN_DATA_G1, LCD_PIN_DATA_G2, LCD_PIN_DATA_G3, LCD_PIN_DATA_G4, LCD_PIN_DATA_G5,
    LCD_PIN_DATA_R0, LCD_PIN_DATA_R1, LCD_PIN_DATA_R2, LCD_PIN_DATA_R3, LCD_PIN_DATA_R4
  };
  for (int i = 0; i < 16; i++) {
    cfg.data_gpio_nums[i] = data_pins[i];
  }
  cfg.flags.fb_in_psram = 1;

  ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&cfg, &panel));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel));

  esp_lcd_rgb_panel_get_frame_buffer(panel, 1, &framebuffer);
  memset(framebuffer, 0, LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t));
}

// interrupt of the panel is allocated on the core that creates it
static void panelInitTask(void *pvParameters) {
  panelInit();
  xSemaphoreGive(panelReady);
  vTaskDelete(NULL);
}

void lcd_panel_start() {
  lgfx::i2c::init(EXPANDER_I2C_PORT, TOUCH_PIN_SDA, TOUCH_PIN_SCL);
  expanderWrite(EXPANDER_REG_MODE, 0xFF);  // all outputs
  expanderOut = 0xFF;                      // LCD on, backlight on, CAN mode (EXIO5 high)
  expanderDigitalWrite(EXIO_TP_RST, LOW);

  // GT911 reset, INT low selects address 0x5D
  pinMode(TOUCH_PIN_ADDR_SEL, OUTPUT);
  digitalWrite(TOUCH_PIN_ADDR_SEL, LOW);
  delay(20);
  expanderDigitalWrite(EXIO_TP_RST, HIGH);
  delay(10);
  pinMode(TOUCH_PIN_ADDR_SEL, INPUT);
  delay(50);

  writeBacklight(brightnessVal);

  panelReady = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(panelInitTask, "LcdInit", 4 * 1024, NULL, 2, NULL, 0);
  xSemaphoreTake(panelReady, portMAX_DELAY);

  auto cfg = touch.config();
  cfg.x_min = 0;
  cfg.x_max = TOUCH_XMAX;
  cfg.y_min = 0;
  cfg.y_max = TOUCH_YMAX;
  cfg.pin_int = TOUCH_PIN_INT;
  cfg.pin_rst = TOUCH_PIN_RST;
  cfg.bus_shared = false;
  cfg.offset_rotation = TOUCH_ROTATION;
  cfg.i2c_port = EXPANDER_I2C_PORT;
  cfg.pin_sda = TOUCH_PIN_SDA;
  cfg.pin_scl = TOUCH_PIN_SCL;
  cfg.freq = TOUCH_FREQ;
  touch.config(cfg);
  [[maybe_unused]] bool ok = touch.init();
#ifdef DEBUG
  Serial.printf("panel %ux%u @%luHz, touch %s\n", LCD_WIDTH, LCD_HEIGHT, (unsigned long)LCD_FREQ, ok ? "found" : "none");
  Serial.printf("heap internal free %u largest %u, psram free %u\n",
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
#endif
}

// LVGL callbacks
void disp_flush_callback(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *px_map) {
#ifdef PERF_MONITOR
  uint32_t t0 = micros();
#endif
  uint16_t *dst = (uint16_t *)framebuffer + area->y1 * LCD_WIDTH + area->x1;
  const uint16_t *src = (const uint16_t *)px_map;
  int32_t w = area->x2 - area->x1 + 1;
  for (int32_t y = area->y1; y <= area->y2; y++) {
    memcpy(dst, src, w * sizeof(uint16_t));
    dst += LCD_WIDTH;
    src += w;
  }
#ifdef PERF_MONITOR
  flushTimeUs += micros() - t0;
#endif
  lv_disp_flush_ready(disp);
}

void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  lgfx::touch_point_t tp;
  data->state = LV_INDEV_STATE_REL;
  if (touch.getTouchRaw(&tp, 1)) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = tp.x;
    data->point.y = tp.y;
  }
}

void setBrightness(int val) {
  if (val <= 55) {
    brightnessVal = 55;
  } else {
    if (val >= 255) {
      brightnessVal = 255;
    } else {
      brightnessVal = val;
    }
  }
  writeBacklight(brightnessVal);
}

#endif  // WAVESHARE_S3_LCD7B
