#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisSwLimitMSetting : public SettingFloat {
    private:
        Motor& _motor1;
        Motor& _motor2;

    public:
        AxisSwLimitMSetting(Motor& motor1, Motor& motor2) : _motor1(motor1), _motor2(motor2) {}

        float getValue() const override {
            return _motor1.getSwLimitMinus();
        }

        void setValue(const float value) override {
            _motor1.setSwLimitMinus(value);
            _motor2.setSwLimitMinus(value);
        }

        const char* getName() const override {
            return "sw_limit_m";
        }

        const char* getTitle() const override {
            return "Software limit minus";
        }

        const char* getDescription() const override {
            return "Minimum position that the axis can reach in a close loop position command";
        }

        const char* getUnit() const override {
            return "deg";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return -5.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 100.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
    };
