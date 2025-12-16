# Copilot Instructions

## Project Overview
This project is developed for the **ESP32-S3-DevKitC-1** board using the **Arduino framework** within the **PlatformIO** environment. The project includes features such as motor control, web server functionality, and RGB LED animations.

## Key Configuration
- **Platform**: espressif32
- **Board**: esp32-s3-devkitc-1
- **Framework**: Arduino
- **Filesystem**: LittleFS
- **Flash Mode**: QIO (Quad I/O)
- **PSRAM Type**: OPI (Octal Peripheral Interface)
- **Flash Size**: 16MB
- **Monitor Speed**: 115200 baud

## Build Flags
The following build flags are used:
- `-D ARDUINO_USB_MODE=1`
- `-D ARDUINO_USB_CDC_ON_BOOT=1`
- `-D BOARD_HAS_PSRAM`

## Libraries
The project depends on the following libraries:
- [PsychicHttp](https://platformio.org/lib/show/12118/PsychicHttp) (v2.1.0)
- [Adafruit NeoPixel](https://platformio.org/lib/show/28/Adafruit%20NeoPixel) (v1.15.2)

## Features
- **Motor Control**: Includes PID control, trajectory generation, and homing functionality.
- **Web Server**: Provides REST API endpoints for controlling and monitoring the system.
- **RGB LED Control**: Uses FastLED for LED animations and error signaling.
- **Task Management**: Utilizes FreeRTOS for multitasking.
- **Logging**: Includes a logger for debugging and monitoring motor performance.

## Directory Structure
- `src/`: Contains the main source code.
- `include/`: Contains header files for modular components.
- `lib/`: Contains additional libraries.
- `data/`: Contains files for the LittleFS filesystem.
- `platformio.ini`: Configuration file for PlatformIO.

## Notes for Copilot
- When generating code, ensure compatibility with the ESP32-S3 and the Arduino framework.
- Use LittleFS for file storage operations.
- Follow the project's existing coding style and modular structure.
- Avoid hardcoding sensitive information like WiFi credentials; use `include/secrets.h` instead.
- Ensure that all generated code respects the memory constraints of the ESP32-S3.

## Additional Resources
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32-S3 Documentation](https://www.espressif.com/en/products/socs/esp32-s3/overview)
- [Arduino Framework Documentation](https://www.arduino.cc/reference/en/)