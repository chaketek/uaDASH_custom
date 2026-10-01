## Waveshare ESP32-S3-Touch-LCD-7B (1024x600)

Build define: `WAVESHARE_S3_LCD7B`

The 7B is built with **ESP-IDF** (Arduino core as a component), which allows tuning the
sdkconfig for the RGB panel. See [docs/adr/0001-build-with-esp-idf.md](docs/adr/0001-build-with-esp-idf.md).

### Build with ESP-IDF (recommended)

Install ESP-IDF v5.4.2 (https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/get-started/), then in the repository root:

```
idf.py set-target esp32s3
idf.py build
idf.py -p <COM port> flash
```

* The first build downloads the dependencies (`main/idf_component.yml`) and takes ~10 minutes.
* Test options: `idf.py "-DAPP_DEFINES=DEMO_DATA;PERF_MONITOR" build`
  * `DEMO_DATA` sweeps all values without an ECU
  * `PERF_MONITOR` shows the LVGL FPS/CPU overlay and prints `PERF fps:.. render:.. flush:..` once per second on the serial port
  * `DEBUG` prints init logs
* Optional 120MHz PSRAM (experimental, ~37Hz refresh, temperature risk, read the ADR first): see `sdkconfig.defaults.psram120`.
* Binaries from CI (`dash_idf_WAVESHARE_S3_LCD7B`) can be flashed with esptool: `esptool --chip esp32s3 write_flash @flash_args`

### Build with Arduino IDE / arduino-cli (fallback)

```
arduino-cli -j16 compile -b esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi --build-property "build.defines=-DBOARD_HAS_PSRAM -DWAVESHARE_S3_LCD7B" --build-property compiler.optimization_flags=-O2 firmware.ino --output-dir ./artifacts -v
```

Works, but the sdkconfig can't be changed: the panel runs at 16MHz (~17.5Hz refresh) and flickers more.

### Notes

* The UI runs at native 1024x600. The SquareLine layout (800x480) is scaled at compile time (`ui_scale.h`: `UI_SX()`/`UI_SY()`), and fonts are 1.25x versions (`ui_font_*_hires.c`, generated with lv_font_conv from the same TTF/OTF). Other boards are unchanged.
* Display driver is ESP-IDF `esp_lcd` RGB panel with bounce buffers (`display_driver_rgb.cpp`), not LovyanGFX. LVGL renders into internal RAM and the flush copies into the PSRAM framebuffer. LovyanGFX is used only for the GT911 touch.
* Pixel clock is chosen from the sdkconfig (`display_driver.h`): ESP-IDF build 24MHz (~26Hz), 120MHz PSRAM 30MHz (~37Hz), Arduino IDE 16MHz (~17.5Hz). Higher clocks make the image jump while the whole screen is redrawn.
* Screen changes use a fade instead of the move animation (full screen moving is too heavy for the framebuffer bandwidth).
* Backlight brightness (swipe up/down) is controlled by the on-board IO expander PWM, no soldering needed.
* CAN uses the on-board TJA1051 transceiver (GPIO19 RX / GPIO20 TX). These pins are shared with the native USB port, the firmware switches EXIO5 to CAN mode, so the native USB Type-C port does not work while the dash is running.
* Flash and debug through the **UART Type-C port** (CH343). The UART slide switch on the board must be set to the USB-UART side.
* If esptool cannot connect, hold BOOT, press RESET, release BOOT and retry.
* The partition table (`partitions.csv`) matches Arduino `app3M_fat9M_16MB`, so saved settings survive switching between the two builds.
