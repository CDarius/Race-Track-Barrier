#pragma once

#include <Adafruit_NeoPixel.h>
#include <limits>

#include "io.h"
#include "devices/rgb_led.hpp"
#include "devices/button.hpp"
#include "devices/unit_encoder.hpp"
#include "motor_control/gantrymotor.hpp"
#include "barrier_config.h"

class ManualHome {
    private:
    UnitEncoder &knob_encoder;
    GantryMotor &motor;
    Adafruit_NeoPixel &start_button_led;
    const barrier_config_t &barrier_config;

    public:
    ManualHome(
        UnitEncoder &knob_encoder,
        GantryMotor &x_motor,
        Adafruit_NeoPixel &start_button_led,
        const barrier_config_t &barrier_config
    ) : knob_encoder(knob_encoder),
        motor(x_motor),
        start_button_led(start_button_led),
        barrier_config(barrier_config)
    {}

    pbio_error_t run_home(CancelToken& cancel_token);
};

