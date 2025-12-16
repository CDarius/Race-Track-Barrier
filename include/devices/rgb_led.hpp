#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN             48

// Common color constants as RGB tuples
#define RGB_COLOR_RED     Adafruit_NeoPixel::Color(255, 0, 0)
#define RGB_COLOR_GREEN   Adafruit_NeoPixel::Color(0, 255, 0)
#define RGB_COLOR_BLUE    Adafruit_NeoPixel::Color(0, 0, 255)
#define RGB_COLOR_WHITE   Adafruit_NeoPixel::Color(255, 255, 255)
#define RGB_COLOR_YELLOW  Adafruit_NeoPixel::Color(255, 255, 0)
#define RGB_COLOR_CYAN    Adafruit_NeoPixel::Color(0, 255, 255)
#define RGB_COLOR_MAGENTA Adafruit_NeoPixel::Color(255, 0, 255)
#define RGB_COLOR_BLACK   Adafruit_NeoPixel::Color(0, 0, 0)

class RGBLed {
public:
    void begin();
    void setColor(const uint32_t color);

    void unrecoverableError();

private:
    Adafruit_NeoPixel led = Adafruit_NeoPixel(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
};
