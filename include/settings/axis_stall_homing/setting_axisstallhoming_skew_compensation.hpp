#pragma once

#include "motor_control\motorwithstallreference.hpp"
#include "settings\setting.hpp"

class AxisStallHomingHomeSkewCompensationSetting : public SettingFloat {
    private:
        gantry_stall_homing_config_t& _config;

    public:
        AxisStallHomingHomeSkewCompensationSetting(gantry_stall_homing_config_t& motorHoming) : _config(motorHoming) {}

        float getValue() const override {
            return _config.skew_pos_compensation;
        }

        void setValue(const float value) override {
            _config.skew_pos_compensation = value;
        }

        const char* getName() const override {
            return "skew_comp";
        }

        const char* getTitle() const override {
            return "Gantry skew compensation";
        }

        const char* getDescription() const override {
            return "Compensation for gantry skew after the homing";
        }

        const char* getUnit() const override {
            return "deg";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return -360.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 360.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1.0;
        }
    };