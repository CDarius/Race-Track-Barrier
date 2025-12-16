#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisIntegralRateSetting : public SettingFloat {
    private:
        Motor& _motor1;
        Motor& _motor2;

    public:
        AxisIntegralRateSetting(Motor& motor1, Motor& motor2) : _motor1(motor1), _motor2(motor2) {}

        float getValue() const override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor1.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            return integral_rate;
        }

        void setValue(const float value) override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor1.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            integral_rate = value;
            _motor1.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);

            _motor2.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            integral_rate = value;
            _motor2.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);
        }

        const char* getName() const override {
            return "intgral_rate";
        }

        const char* getTitle() const override {
            return "Integral Rate";
        }

        const char* getDescription() const override {
            return "Maximum integral change per single update";
        }

        const char* getUnit() const override {
            return "deg";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 0.01;
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
            return 0.01;
        }
    };