# Race Track Barrier

A start barrier system for a Lego race track, built on the ESP32-S3-DevKitC-1 using the Arduino framework and PlatformIO. It features motor control, web server REST APIs, RGB LED animations, and robust task management for reliable operation.

## Hardware
- **Board**: ESP32-S3-DevKitC-1
- **Flash**: 16MB QIO
- **PSRAM**: OPI

## Software Stack
- **Platform**: PlatformIO (espressif32)
- **Framework**: Arduino
- **Libraries**:
	- [PsychicHttp](https://platformio.org/lib/show/12118/PsychicHttp) (v2.1.0)
	- [Adafruit NeoPixel](https://platformio.org/lib/show/28/Adafruit%20NeoPixel) (v1.15.2)
	- FastLED (for RGB control)

## Directory Structure
- `src/` — Main source code
- `include/` — Modular headers
- `lib/` — Additional libraries
- `data/` — LittleFS web assets
- `settings-webapp/` — Web app for configure the race track barrier

## Getting Started
1. **Clone the repository**
2. **Install PlatformIO** ([docs](https://docs.platformio.org/))
3. **Configure WiFi credentials** in `include/secrets.h`
4. **Build and upload**:
	 ```sh
	 platformio run --target upload --upload-port <COM_PORT>
	 ```
5. **Access the web interface** at the device's IP address

## Configuration
- All settings are modular and grouped in `include/settings/`
- Web API endpoints documented in `src/api_server/`
- RGB LED and motor logic in `src/devices/` and `src/motor_control/`

## Notes
- Avoid hardcoding sensitive data; use `include/secrets.h`
- Designed for ESP32-S3 memory constraints

## License
MIT
