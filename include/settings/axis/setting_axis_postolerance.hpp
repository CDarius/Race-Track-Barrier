#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisPosToleranceSetting : public SettingFloat {
    private:
        Motor& _motor1;
        Motor& _motor2;

    public:
        AxisPosToleranceSetting(Motor& motor1, Motor& motor2) : _motor1(motor1), _motor2(motor2) {}

        float getValue() const override {
            float speed,position;
            _motor1.get_target_tolerances(&speed, &position);
            return position;
        }

        void setValue(const float value) override {
            float speed, position;

            _motor1.get_target_tolerances(&speed, &position);
            position = value;
            _motor1.set_target_tolerances(speed, position);

            _motor2.get_target_tolerances(&speed, &position);
            position = value;
            _motor2.set_target_tolerances(speed, position);
        }

        const char* getName() const override {
            return "pos_tolerance";
        }

        const char* getTitle() const override {
            return "Position Tolerance";
        }

        const char* getDescription() const override {
            return "Maximum position deviation tolerance at the end of a close loop movement";
        }

        const char* getUnit() const override {
            return "deg";
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