#pragma once

#include "barrier_config.h"
#include "settings\setting.hpp"

class BarrierHoldTimeSetting : public SettingUInt16 {
    private:
        barrier_config_t& _config;

    public:
        BarrierHoldTimeSetting(barrier_config_t& config) : _config(config) {}

        uint16_t getValue() const override {
            return _config.barrier_hold_time;
        }

        void setValue(const uint16_t value) override {
            _config.barrier_hold_time = value;
        }

        const char* getName() const override {
            return "hold_time";
        }

        const char* getTitle() const override {
            return "Barrier lowered hold time";
        }

        const char* getDescription() const override {
            return "Time to hold the barrier in the lowered position";
        }

        const char* getUnit() const override {
            return "seconds";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const uint16_t getMinValue() const override {
            return 3;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const uint16_t getMaxValue() const override {
            return 100;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const uint16_t getChangeStep() const override {
            return 5;
        }
    };
