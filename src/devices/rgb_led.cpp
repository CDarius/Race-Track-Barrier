#include "devices/rgb_led.hpp"

void RGBLed::begin() {
    led.begin();
    led.setBrightness(100); 
    led.show();
}

void RGBLed::setColor(const uint32_t color) {
    led.setPixelColor(0, color);
    led.show();
}

/**
 * @brief Blink the LED in red color to indicate an unrecoverable error.
 * This function will run indefinitely, blinking the LED every 500 milliseconds.
 */
void RGBLed::unrecoverableError() {
    while (true)
    {        
        led.setPixelColor(0, RGB_COLOR_RED);
        led.show();
        delay(200);
        led.setPixelColor(0, RGB_COLOR_BLACK);
        led.show();
        delay(200);
    }
    
}
