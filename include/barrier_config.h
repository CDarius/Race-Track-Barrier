#pragma once

struct barrier_config_t {
    float manual_homing_speed;  // Speed to use during manual homing (motor units/second)
    float jog_multiplier;       // Multiplier to apply to the knob encoder during manual homing
};