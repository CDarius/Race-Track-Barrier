#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisPidKdSetting : public SettingUInt16 {
    private:
        Motor& _motor1;
        Motor& _motor2;

    public:
        AxisPidKdSetting(Motor& motor1, Motor& motor2) : _motor1(motor1), _motor2(motor2) {}

        uint16_t getValue() const override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor1.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            return kd;
        }

        void setValue(const uint16_t value) override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor1.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            kd = value;
            _motor1.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);

            _motor2.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            kd = value;
            _motor2.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);
        }

        const char* getName() const override {
            return "pid_kd";
        }

        const char* getTitle() const override {
            return "PID Kd";
        }

        const char* getDescription() const override {
            return "Derivative gain for the PID controller";
        }

        const char* getUnit() const override {
            return "";
        }

        const uint16_t getMinValue() const override {
            return 0;
        }

        const uint16_t getMaxValue() const override {
            return 1000;
        }
    };
