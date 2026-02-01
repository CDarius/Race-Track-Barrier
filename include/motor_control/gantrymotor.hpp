#pragma once

#include <Arduino.h>
#include "motor.hpp"
#include "motor_control/logger.hpp"

class GantryMotor {
    private:
        const char* _name;
        Motor& _motor1;
        Motor& _motor2;
        float _skew_pos_compensation; // Position correction to apply at the second motor to compensate the gantry skew (motor units)
        bool _referenced = false;

    public:
        GantryMotor(Motor& motor1, Motor& motor2)
            : _motor1(motor1), _motor2(motor2), _skew_pos_compensation(0.0f) {}

        Motor& motor1() {
            return _motor1;
        }

        Motor& motor2() {
            return _motor2;
        }


        void begin(const char* name);

        const char* name() const {
            return _name;
        }

        float getSkewCompensation() const {
            return _skew_pos_compensation;
        }

        void setSkewCompensation(float compensation) {
            _skew_pos_compensation = compensation;
        }

        virtual bool referenced() const {
            return _referenced;
        }

        float angle() const;
        float speed() const;
        void reset_angle(float angle);

        void stop();
        void brake();
        void hold();

        /**
        Runs the motor at a constant speed.

        The motor accelerates to the given speed and keeps running at this speed until you give a new command.

        :param speed: Speed of the motor in deg/s
        */        
        pbio_error_t run(float speed) {
            return _motor1.run(speed);
        }

        /**
        Runs the motor at a constant speed towards a given target angle.

        The direction of rotation is automatically selected based on the target angle. It does not matter if speed is positive or negative.

        :param speed: Speed of the motor in deg/s
        :param target_angle: Angle that the motor should rotate to in deg
        :param then: What to do after coming to a standstill
        :param wait: Wait for the maneuver to complete before continuing with the rest of the program
        */
        pbio_error_t run_target(float speed, float target_angle, pbio_actuation_t then = PBIO_ACTUATION_HOLD, bool wait = true, CancelToken* cancel_token = nullptr) {
            return _motor1.run_target(speed, target_angle, then, wait, cancel_token);
        }

        /** 
        Tracks a target angle. 

        This is similar to run_target(), but the usual smooth acceleration is skipped: it will move 
        to the target angle as fast as possible. This method is useful if you want to continuously 
        change the target angle.

        :param target_angle: Target angle that the motor should rotate to in deg
        */
        void track_target(float target_angle) {
            _motor1.track_target(target_angle);
        }

        /**
        Set actuation limit in percentage

        :param actuation: Maximum actuation percentage (0 to 100%)
        */
        void set_actuation_limit(uint8_t actuation) {
            _motor1.set_actuation_limit(actuation);
            _motor2.set_actuation_limit(actuation);
        }

        void get_target_tolerances(float *speed, float *position) const {
            _motor1.get_target_tolerances(speed, position);
        }

        float get_speed_limit() const {
            return _motor1.get_speed_limit();
        }

        float getSwLimitMinus() const {
            return _motor1.getSwLimitMinus();
        }

        float getSwLimitPlus() const {
            return _motor1.getSwLimitPlus();
        }

        // New functions for axis info
        float get_counts_per_unit() const {
            return _motor1.get_counts_per_unit();
        }

        float get_acceleration_limit() const {
            return _motor1.get_acceleration_limit();
        }

        float get_actuation_limit() const {
            return _motor1.get_actuation_limit();
        }

        void update();
        
        PBIOLogger* get_logger() {
            return _motor1.get_logger();
        }
};