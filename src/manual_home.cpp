#include <Arduino.h>
#include "manual_home.hpp"

pbio_error_t ManualHome::run_home(CancelToken& cancel_token) {
    Button start_button;
    Button knob_button;

    // Start button blink variables
    uint32_t start_button_blink_time = millis();
    uint32_t start_button_blink_interval = 300;
    bool start_button_staus = false;

    // Start with up/down barrier function
    bool up_down_function = true;
    bool function_changed = true;

    // Set angle setpoint to undefined and cleare knob value
    float x_angle_setpoint = std::numeric_limits<float>::quiet_NaN();
    int16_t knob_value = INT16_MIN;
    knob_encoder.clearValue();

    // Start positon holding
    motor.track_target(motor.angle());

    while (!start_button.isHolding()) {
        uint32_t now = millis();

        // Check for cancellation
        if (cancel_token.isCancelled()) {
            motor.stop();

            // Switch off knob LEDs
            knob_encoder.setLEDColor(0, RGB_COLOR_BLACK);
            knob_encoder.setLEDColor(1, RGB_COLOR_BLACK);

            // Switch off start button LED
            start_button_led.setPixelColor(0, RGB_COLOR_BLACK);
            start_button_led.show();

            return PBIO_ERROR_CANCELED;
        }

        // Blink start button LED
        if (now >= start_button_blink_time) {
            start_button_blink_time = now + start_button_blink_interval;
            start_button_staus = !start_button_staus;
            start_button_led.setPixelColor(0, start_button_staus ? RGB_COLOR_RED : RGB_COLOR_BLACK);
            start_button_led.show();
        }

        // Update start button state
        start_button.update(now, START_BUTTON_PRESSED);

        // Update knob button state
        bool knob_pressed;
        knob_encoder.isButtonPressed(knob_pressed);
        knob_button.update(now, knob_pressed);

        // Update knob delta value
        int16_t new_knob_value;
        int16_t knob_delta = 0;
        if (knob_encoder.getValue(new_knob_value)) {
            new_knob_value /= 2; // One count per encoder detent
            if (knob_value == INT16_MIN) {
                knob_value = new_knob_value;
            } else {
                knob_delta = new_knob_value - knob_value;
                knob_value = new_knob_value;
            }
        } else {
            knob_value = INT16_MIN;
        }

        // Toggle function when knob button is short clicked
        if (knob_button.wasClicked()) {
            up_down_function = !up_down_function;
            function_changed = true;
        }

        if (up_down_function) {
            // Move the barrier up and down using the knob encoder
            if (function_changed) {
                knob_encoder.setLEDColor(0, RGB_COLOR_BLUE);
                knob_encoder.setLEDColor(1, RGB_COLOR_BLACK);
            }

            // Move down the barrier when the know is held down
            if (knob_button.isHolding()) {
                motor.run(-barrier_config.manual_homing_speed);

                // Wait until the knob is released
                do {
                    delay(50);
                    knob_encoder.isButtonPressed(knob_pressed);
                    knob_button.update(millis(), knob_pressed);
                } while (knob_button.isHolding());

                // Stop the motor
                motor.hold();

                // Abort any involuntary knob rotation and reset setpoint for X-Axis jog
                knob_value = INT16_MIN;
                x_angle_setpoint = std::numeric_limits<float>::quiet_NaN();
                continue;
            }

            // Jog the barrier up/down with the knob rotation
            if (knob_delta != 0) {
                if (std::isnan(x_angle_setpoint)) {
                    x_angle_setpoint = motor.angle();
                }
                x_angle_setpoint += (float)knob_delta * barrier_config.jog_multiplier;
                motor.track_target(x_angle_setpoint);
            }
        } else {
            // Adjust skew compensation using the knob encoder
            if (function_changed) {
                knob_encoder.setLEDColor(0, RGB_COLOR_BLACK);
                knob_encoder.setLEDColor(1, RGB_COLOR_MAGENTA);
            }

            motor.setSkewCompensation(motor.getSkewCompensation() + (float)knob_delta);
        }

        function_changed = false;
        delay(20);
    }
    motor.reset_angle(0.0f);

    // Switch off knob LEDs
    knob_encoder.setLEDColor(0, RGB_COLOR_BLACK);
    knob_encoder.setLEDColor(1, RGB_COLOR_BLACK);

    // Switch off start button LED
    start_button_led.setPixelColor(0, RGB_COLOR_BLACK);
    start_button_led.show();

    return PBIO_SUCCESS;
}
