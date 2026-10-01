## Waveshare ESP32-S3-Touch-LCD-7B (1024x600)

Build define: `WAVESHARE_S3_LCD7B`

```
arduino-cli -j16 compile -b esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi --build-property "build.defines=-DBOARD_HAS_PSRAM -DWAVESHARE_S3_LCD7B" --build-property compiler.optimization_flags=-O2 firmware.ino --output-dir ./artifacts -v
```

Notes:

* The UI is still 800x480, drawn centered on the 1024x600 panel (black border around it). Touch coordinates are mapped to the UI area.
* Backlight brightness (swipe up/down) is controlled by the on-board IO expander PWM, no soldering needed.
* CAN uses the on-board TJA1051 transceiver (GPIO19 RX / GPIO20 TX). These pins are shared with the native USB port, the firmware switches EXIO5 to CAN mode, so the native USB Type-C port does not work while the dash is running.
* Flash and debug through the **UART Type-C port** (CH343). The UART slide switch on the board must be set to the USB-UART side.
* If esptool cannot connect, hold BOOT, press RESET, release BOOT and retry.
* Pixel clock is 16MHz: with LovyanGFX the framebuffer is read from PSRAM without bounce buffers, higher clocks make the image shift.
