#pragma once

#include "barrier_config.h"
#include "settings\setting.hpp"

class BarrierRaisingSpeedSetting : public SettingFloat {
    private:
        barrier_config_t& _config;

    public:
        BarrierRaisingSpeedSetting(barrier_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.barrier_raise_speed;
        }

        void setValue(const float value) override {
            _config.barrier_raise_speed = value;
        }

        const char* getName() const override {
            return "raising_speed";
        }

        const char* getTitle() const override {
            return "Barrier raising speed";
        }

        const char* getDescription() const override {
            return "Axis speed used to raise the barrier";
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
            return 1000.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 5.0;
        }
    };
