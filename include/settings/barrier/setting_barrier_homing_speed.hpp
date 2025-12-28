#pragma once

#include "barrier_config.h"
#include "settings\setting.hpp"

class BarrierHomingSpeedSetting : public SettingFloat {
    private:
        barrier_config_t& _config;

    public:
        BarrierHomingSpeedSetting(barrier_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.manual_homing_speed;
        }

        void setValue(const float value) override {
            _config.manual_homing_speed = value;
        }

        const char* getName() const override {
            return "home_speed";
        }

        const char* getTitle() const override {
            return "Manual homing speed";
        }

        const char* getDescription() const override {
            return "Axis speed when the knob is held down during manual homing";
        }

        const char* getUnit() const override {
            return "deg/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 10.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 500.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 5.0;
        }
    };
