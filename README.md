# coach-in firmware
## Arduino
### Dependencies

Clone these repos into your `$HOME/Documents/Arduino/libraries`.
- [Arduino_SPI-Stack](https://github.com/0x0c/Arduino_SPI-Stack)
- [Adafruit-PWM-Servo-Driver-Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)
- [multi-ems@multi-ems-cpp](https://github.com/rkmtlab/multi-ems/tree/multi-ems-cpp)

## ESP32
### Requirements

- esp-idf v3.1

### Dependencies

- [esp32-MPU-driver](https://github.com/natanaeljr/esp32-MPU-driver)
- [esp32-I2Cbus](https://github.com/natanaeljr/esp32-I2Cbus)
- [WS2812-Utility](https://github.com/0x0c/WS2812-Utility)
- [ESP32_StreamLogger](https://github.com/0x0c/ESP32_StreamLogger)
- [FreeRTOS-cpp_task](https://github.com/0x0c/FreeRTOS-cpp_task)
- [esp32-snippets/cpp_utils@11125da](https://github.com/nkolban/esp32-snippets/tree/11125da923a4921e17f6524187587ad4120cd8d0) (Already installed this repo.)

### Setting up

1. Clone this repo.
2. Update submodules.

```
$ git submodule update --init
```
