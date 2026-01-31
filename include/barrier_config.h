#pragma once

struct barrier_config_t {
    float manual_homing_speed;  // Speed to use during manual homing (deg/second)
    float jog_multiplier;       // Multiplier to apply to the knob encoder during manual homing
    float barrier_lower_speed;  // Speed to lower the barrier (deg/second)
    float barrier_raise_speed;  // Speed to raise the barrier (deg/second)
    uint8_t barrier_raise_power;  // Max power to apply when raising the barrier (% of max power)
    uint16_t barrier_hold_time; // Time to hold the barrier in the lowern position (seconds)
};