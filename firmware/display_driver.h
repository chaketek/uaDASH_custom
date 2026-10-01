#pragma once

#include "config.h"
#include "lvgl.h"
#include "lv_conf.h"

#if defined(WAVESHARE_S3_LCD7) || defined(WAVESHARE_S3_LCD5)
#include <ESP_IOExpander_Library.h>
#endif

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

#ifdef JC8048W550C

#define LCD_PIN_BACKLIGHT   GPIO_NUM_2

// resolution
#define LCD_WIDTH   800U
#define LCD_HEIGHT   480U

#define LCD_PIN_HENABLE     GPIO_NUM_40
#define LCD_PIN_VSYNC       GPIO_NUM_41
#define LCD_PIN_HSYNC       GPIO_NUM_39
#define LCD_PIN_PCLK        GPIO_NUM_42

// RGB565 data pins
#define LCD_PIN_DATA_R0     GPIO_NUM_45
#define LCD_PIN_DATA_R1     GPIO_NUM_48
#define LCD_PIN_DATA_R2     GPIO_NUM_47
#define LCD_PIN_DATA_R3     GPIO_NUM_21
#define LCD_PIN_DATA_R4     GPIO_NUM_14
  
#define LCD_PIN_DATA_G0     GPIO_NUM_5
#define LCD_PIN_DATA_G1     GPIO_NUM_6
#define LCD_PIN_DATA_G2     GPIO_NUM_7
#define LCD_PIN_DATA_G3     GPIO_NUM_15
#define LCD_PIN_DATA_G4     GPIO_NUM_16
#define LCD_PIN_DATA_G5     GPIO_NUM_4

#define LCD_PIN_DATA_B0     GPIO_NUM_8
#define LCD_PIN_DATA_B1     GPIO_NUM_3
#define LCD_PIN_DATA_B2     GPIO_NUM_46
#define LCD_PIN_DATA_B3     GPIO_NUM_9
#define LCD_PIN_DATA_B4     GPIO_NUM_1

// timing and polarity
#define LCD_FREQ                16000000U
#define LCD_PCLK_ACTIVE_NEG     1
#define LCD_DE_IDLE_HIGH        1
#define LCD_PCLK_IDLE_HIGH      1

#define LCD_HSYNC_POLARITY      0
#define LCD_HSYNC_FRONT_PORCH   8
#define LCD_HSYNC_PULSE_WIDTH   4
#define LCD_HSYNC_BACK_PORCH    8

#define LCD_VSYNC_POLARITY      0
#define LCD_VSYNC_FRONT_PORCH   8
#define LCD_VSYNC_PULSE_WIDTH   4
#define LCD_VSYNC_BACK_PORCH    8

// touch
#define TOUCH_XMAX  LCD_WIDTH - 1
#define TOUCH_YMAX  LCD_HEIGHT - 1

#define TOUCH_PIN_INT   GPIO_NUM_NC
#define TOUCH_PIN_RST   GPIO_NUM_38
#define TOUCH_PIN_SDA   GPIO_NUM_19
#define TOUCH_PIN_SCL   GPIO_NUM_20

#define TOUCH_FREQ      400000U
#define TOUCH_ROTATION  0

// JC8048W550C end

#elif defined(WAVESHARE_S3_LCD7)

#define LCD_PIN_BACKLIGHT   GPIO_NUM_6

// resolution
#define LCD_WIDTH   800U
#define LCD_HEIGHT   480U

#define LCD_PIN_HENABLE     GPIO_NUM_5
#define LCD_PIN_VSYNC       GPIO_NUM_3
#define LCD_PIN_HSYNC       GPIO_NUM_46
#define LCD_PIN_PCLK        GPIO_NUM_7

// RGB565 data pins
#define LCD_PIN_DATA_R0     GPIO_NUM_1
#define LCD_PIN_DATA_R1     GPIO_NUM_2
#define LCD_PIN_DATA_R2     GPIO_NUM_42
#define LCD_PIN_DATA_R3     GPIO_NUM_41
#define LCD_PIN_DATA_R4     GPIO_NUM_40
  
#define LCD_PIN_DATA_G0     GPIO_NUM_39
#define LCD_PIN_DATA_G1     GPIO_NUM_0
#define LCD_PIN_DATA_G2     GPIO_NUM_45
#define LCD_PIN_DATA_G3     GPIO_NUM_48
#define LCD_PIN_DATA_G4     GPIO_NUM_47
#define LCD_PIN_DATA_G5     GPIO_NUM_21

#define LCD_PIN_DATA_B0     GPIO_NUM_14
#define LCD_PIN_DATA_B1     GPIO_NUM_38
#define LCD_PIN_DATA_B2     GPIO_NUM_18
#define LCD_PIN_DATA_B3     GPIO_NUM_17
#define LCD_PIN_DATA_B4     GPIO_NUM_10

// timing and polarity
#define LCD_FREQ                14000000U
#define LCD_PCLK_ACTIVE_NEG     1
#define LCD_DE_IDLE_HIGH        1
#define LCD_PCLK_IDLE_HIGH      1

#define LCD_HSYNC_POLARITY      0
#define LCD_HSYNC_FRONT_PORCH   8
#define LCD_HSYNC_PULSE_WIDTH   4
#define LCD_HSYNC_BACK_PORCH    8

#define LCD_VSYNC_POLARITY      0
#define LCD_VSYNC_FRONT_PORCH   8
#define LCD_VSYNC_PULSE_WIDTH   4
#define LCD_VSYNC_BACK_PORCH    8

// touch
#define TOUCH_XMAX  LCD_WIDTH - 1
#define TOUCH_YMAX  LCD_HEIGHT - 1

#define TOUCH_PIN_INT   GPIO_NUM_NC
#define TOUCH_PIN_RST   GPIO_NUM_NC
#define TOUCH_PIN_SDA   GPIO_NUM_8
#define TOUCH_PIN_SCL   GPIO_NUM_9

#define TOUCH_FREQ      400000U
#define TOUCH_ROTATION  0

// WAVESHARE_S3_LCD7 end

#elif defined(WAVESHARE_S3_LCD5)

#define LCD_PIN_BACKLIGHT   GPIO_NUM_6

// resolution
#define LCD_WIDTH   800U
#define LCD_HEIGHT   480U

#define LCD_PIN_HENABLE     GPIO_NUM_5
#define LCD_PIN_VSYNC       GPIO_NUM_3
#define LCD_PIN_HSYNC       GPIO_NUM_46
#define LCD_PIN_PCLK        GPIO_NUM_7

// RGB565 data pins
#define LCD_PIN_DATA_R0     GPIO_NUM_1
#define LCD_PIN_DATA_R1     GPIO_NUM_2
#define LCD_PIN_DATA_R2     GPIO_NUM_42
#define LCD_PIN_DATA_R3     GPIO_NUM_41
#define LCD_PIN_DATA_R4     GPIO_NUM_40
  
#define LCD_PIN_DATA_G0     GPIO_NUM_39
#define LCD_PIN_DATA_G1     GPIO_NUM_0
#define LCD_PIN_DATA_G2     GPIO_NUM_45
#define LCD_PIN_DATA_G3     GPIO_NUM_48
#define LCD_PIN_DATA_G4     GPIO_NUM_47
#define LCD_PIN_DATA_G5     GPIO_NUM_21

#define LCD_PIN_DATA_B0     GPIO_NUM_14
#define LCD_PIN_DATA_B1     GPIO_NUM_38
#define LCD_PIN_DATA_B2     GPIO_NUM_18
#define LCD_PIN_DATA_B3     GPIO_NUM_17
#define LCD_PIN_DATA_B4     GPIO_NUM_10

// timing and polarity
#define LCD_FREQ                16000000U
#define LCD_PCLK_ACTIVE_NEG     1
#define LCD_DE_IDLE_HIGH        1
#define LCD_PCLK_IDLE_HIGH      1

#define LCD_HSYNC_POLARITY      0
#define LCD_HSYNC_FRONT_PORCH   8
#define LCD_HSYNC_PULSE_WIDTH   4
#define LCD_HSYNC_BACK_PORCH    8

#define LCD_VSYNC_POLARITY      0
#define LCD_VSYNC_FRONT_PORCH   12
#define LCD_VSYNC_PULSE_WIDTH   4
#define LCD_VSYNC_BACK_PORCH    8

// touch
#define TOUCH_XMAX  LCD_WIDTH - 1
#define TOUCH_YMAX  LCD_HEIGHT - 1

#define TOUCH_PIN_INT   GPIO_NUM_NC
#define TOUCH_PIN_RST   GPIO_NUM_NC
#define TOUCH_PIN_SDA   GPIO_NUM_8
#define TOUCH_PIN_SCL   GPIO_NUM_9

#define TOUCH_FREQ      400000U
#define TOUCH_ROTATION  0

// WAVESHARE_S3_LCD5 end

#elif defined(WAVESHARE_S3_LCD7B)

// Backlight is driven by IO expander PWM (no direct GPIO)
#define LCD_PIN_BACKLIGHT   GPIO_NUM_NC

// UI resolution (SquareLine project is 800x480)
#define LCD_WIDTH   800U
#define LCD_HEIGHT   480U

// physical panel resolution, UI is drawn centered
#define LCD_PANEL_WIDTH     1024U
#define LCD_PANEL_HEIGHT    600U

#define LCD_PIN_HENABLE     GPIO_NUM_5
#define LCD_PIN_VSYNC       GPIO_NUM_3
#define LCD_PIN_HSYNC       GPIO_NUM_46
#define LCD_PIN_PCLK        GPIO_NUM_7

// RGB565 data pins
#define LCD_PIN_DATA_R0     GPIO_NUM_1
#define LCD_PIN_DATA_R1     GPIO_NUM_2
#define LCD_PIN_DATA_R2     GPIO_NUM_42
#define LCD_PIN_DATA_R3     GPIO_NUM_41
#define LCD_PIN_DATA_R4     GPIO_NUM_40

#define LCD_PIN_DATA_G0     GPIO_NUM_39
#define LCD_PIN_DATA_G1     GPIO_NUM_0
#define LCD_PIN_DATA_G2     GPIO_NUM_45
#define LCD_PIN_DATA_G3     GPIO_NUM_48
#define LCD_PIN_DATA_G4     GPIO_NUM_47
#define LCD_PIN_DATA_G5     GPIO_NUM_21

#define LCD_PIN_DATA_B0     GPIO_NUM_14
#define LCD_PIN_DATA_B1     GPIO_NUM_38
#define LCD_PIN_DATA_B2     GPIO_NUM_18
#define LCD_PIN_DATA_B3     GPIO_NUM_17
#define LCD_PIN_DATA_B4     GPIO_NUM_10

// timing and polarity (ST7262, Waveshare demo uses 30MHz with bounce buffers)
// LovyanGFX keeps porch/pulse as int8_t (max 127), so the vendor's
// hsync 162/152 can't be used as is; the panel runs in DE mode anyway.
// Above ~20MHz the PSRAM framebuffer DMA underruns and the image shifts.
#define LCD_FREQ                16000000U
#define LCD_PCLK_ACTIVE_NEG     1
#define LCD_DE_IDLE_HIGH        0
#define LCD_PCLK_IDLE_HIGH      0

#define LCD_HSYNC_POLARITY      0
#define LCD_HSYNC_FRONT_PORCH   48
#define LCD_HSYNC_PULSE_WIDTH   40
#define LCD_HSYNC_BACK_PORCH    120

#define LCD_VSYNC_POLARITY      0
#define LCD_VSYNC_FRONT_PORCH   3
#define LCD_VSYNC_PULSE_WIDTH   45
#define LCD_VSYNC_BACK_PORCH    13

// touch (GT911 reports panel coordinates)
#define TOUCH_XMAX  LCD_PANEL_WIDTH - 1
#define TOUCH_YMAX  LCD_PANEL_HEIGHT - 1

#define TOUCH_PIN_INT   GPIO_NUM_NC   // GPIO4, used only for address select on reset
#define TOUCH_PIN_RST   GPIO_NUM_NC   // IO expander EXIO1
#define TOUCH_PIN_SDA   GPIO_NUM_8
#define TOUCH_PIN_SCL   GPIO_NUM_9

#define TOUCH_FREQ      400000U
#define TOUCH_ROTATION  0

// IO expander (I2C 0x24, shared bus with touch)
#define EXPANDER_I2C_PORT   1
#define EXPANDER_I2C_ADDR   0x24
#define EXPANDER_REG_MODE   0x02
#define EXPANDER_REG_OUTPUT 0x03
#define EXPANDER_REG_PWM    0x05

#define EXIO_TP_RST     1
#define EXIO_DISP       2
#define EXIO_LCD_RST    3
#define EXIO_SD_CS      4
#define EXIO_USB_SEL    5   // low: USB, high: CAN
#define EXIO_LCD_VDD_EN 6

#define TOUCH_PIN_ADDR_SEL  GPIO_NUM_4

// WAVESHARE_S3_LCD7B end

#elif defined(Sunton_S3_LCD7)

#define LCD_PIN_BACKLIGHT   GPIO_NUM_2

// resolution
#define LCD_WIDTH   800U
#define LCD_HEIGHT   480U

#define LCD_PIN_HENABLE     GPIO_NUM_41
#define LCD_PIN_VSYNC       GPIO_NUM_40
#define LCD_PIN_HSYNC       GPIO_NUM_39
#define LCD_PIN_PCLK        GPIO_NUM_42

// RGB565 data pins
#define LCD_PIN_DATA_R0     GPIO_NUM_14
#define LCD_PIN_DATA_R1     GPIO_NUM_21
#define LCD_PIN_DATA_R2     GPIO_NUM_47
#define LCD_PIN_DATA_R3     GPIO_NUM_48
#define LCD_PIN_DATA_R4     GPIO_NUM_45

#define LCD_PIN_DATA_G0     GPIO_NUM_9
#define LCD_PIN_DATA_G1     GPIO_NUM_46
#define LCD_PIN_DATA_G2     GPIO_NUM_3
#define LCD_PIN_DATA_G3     GPIO_NUM_8
#define LCD_PIN_DATA_G4     GPIO_NUM_16
#define LCD_PIN_DATA_G5     GPIO_NUM_1

#define LCD_PIN_DATA_B0     GPIO_NUM_15
#define LCD_PIN_DATA_B1     GPIO_NUM_7
#define LCD_PIN_DATA_B2     GPIO_NUM_6
#define LCD_PIN_DATA_B3     GPIO_NUM_5
#define LCD_PIN_DATA_B4     GPIO_NUM_4

// timing and polarity
#define LCD_FREQ                14000000U
#define LCD_PCLK_ACTIVE_NEG     1
#define LCD_DE_IDLE_HIGH        0
#define LCD_PCLK_IDLE_HIGH      0

#define LCD_HSYNC_POLARITY      0
#define LCD_HSYNC_FRONT_PORCH   20
#define LCD_HSYNC_PULSE_WIDTH   30
#define LCD_HSYNC_BACK_PORCH    16

#define LCD_VSYNC_POLARITY      0
#define LCD_VSYNC_FRONT_PORCH   22
#define LCD_VSYNC_PULSE_WIDTH   13
#define LCD_VSYNC_BACK_PORCH    10

// touch
#define TOUCH_XMAX  LCD_WIDTH - 1
#define TOUCH_YMAX  LCD_HEIGHT - 1

#define TOUCH_PIN_INT   GPIO_NUM_NC
#define TOUCH_PIN_RST   GPIO_NUM_38
#define TOUCH_PIN_SDA   GPIO_NUM_19
#define TOUCH_PIN_SCL   GPIO_NUM_20

#define TOUCH_FREQ      400000U
#define TOUCH_ROTATION  0

// Sunton_S3_LCD7 end

#else
#error "Please choose LCD type in config.h"
#endif

#ifndef LCD_PANEL_WIDTH
#define LCD_PANEL_WIDTH     LCD_WIDTH
#define LCD_PANEL_HEIGHT    LCD_HEIGHT
#endif

// UI offset on the physical panel
#define LCD_OFFSET_X    ((LCD_PANEL_WIDTH - LCD_WIDTH) / 2)
#define LCD_OFFSET_Y    ((LCD_PANEL_HEIGHT - LCD_HEIGHT) / 2)



class LCD_Panel : public lgfx::LGFX_Device {
  lgfx::Bus_RGB _bus_instance;
  lgfx::Touch_GT911 _touch_instance;
  lgfx::Panel_RGB _panel_instance;

  public: LCD_Panel(void);
};

extern LCD_Panel display;
extern int brightnessVal;

#ifdef __cplusplus
extern "C" {
#endif

void lcd_panel_start();

void setBrightness(int val);
void disp_flush_callback(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *px_map);
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

#ifdef __cplusplus
} /*extern "C"*/
#endif

