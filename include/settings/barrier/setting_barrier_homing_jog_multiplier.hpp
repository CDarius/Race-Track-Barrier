#pragma once

#include "barrier_config.h"
#include "settings\setting.hpp"

class BarrierJogMultiplierSetting : public SettingFloat {
    private:
        barrier_config_t& _config;

    public:
        BarrierJogMultiplierSetting(barrier_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.jog_multiplier;
        }

        void setValue(const float value) override {
            _config.jog_multiplier = value;
        }

        const char* getName() const override {
            return "jog_multiplier";
        }

        const char* getTitle() const override {
            return "Manual homing jog multiplier";
        }

        const char* getDescription() const override {
            return "Multiplier applied to the knob encoder during manual homing";
        }

        const char* getUnit() const override {
            return "";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 1.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 10.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
    };
