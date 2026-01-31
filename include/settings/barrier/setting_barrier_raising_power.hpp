#pragma once

#include "barrier_config.h"
#include "settings\setting.hpp"

class BarrierRaisingPowerSetting : public SettingUInt8 {
    private:
        barrier_config_t& _config;

    public:
        BarrierRaisingPowerSetting(barrier_config_t& config) : _config(config) {}

        uint8_t getValue() const override {
            return _config.barrier_raise_power;
        }

        void setValue(const uint8_t value) override {
            _config.barrier_raise_power = value;
        }

        const char* getName() const override {
            return "raising_power";
        }

        const char* getTitle() const override {
            return "Barrier raising power";
        }

        const char* getDescription() const override {
            return "Axis power limit used to raise the barrier";
        }

        const char* getUnit() const override {
            return "%";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const uint8_t getMinValue() const override {
            return 40;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const uint8_t getMaxValue() const override {
            return 100;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const uint8_t getChangeStep() const override {
            return 5;
        }
    };
