#include "motor_control/gantrymotor.hpp"

void GantryMotor::begin(const char* name) {
    _name = name;
    _skew_pos_compensation = motor2().angle() - motor1().angle();
}

/**
Gets the rotation angle of the motor (deg)
*/
float GantryMotor::angle() const {
    return (_motor1.angle() + _motor2.angle() - _skew_pos_compensation) / 2.0f;
}


/**
Gets the speed of the motor in deg/s
*/
float GantryMotor::speed() const {
    return _motor1.speed();
}

void GantryMotor::reset_angle(float angle) {
    _motor1.reset_angle(angle);
    _motor2.reset_angle(angle);
    _skew_pos_compensation = 0.0f;
    _referenced = true;
}

/**
Stops the motor and lets it spin freely

The motor gradually stops due to friction
*/
void GantryMotor::stop() {
    _motor1.stop();
    _motor2.stop();
}

/**
Passively brakes the motor

The motor stops due to friction, plus the voltage that is generated while the motor is still moving
*/
void GantryMotor::brake() {
    _motor1.brake();
    _motor2.brake();
}

/**
Stops the motor and actively holds it at its current angle.
*/
void GantryMotor::hold() {
    _motor1.hold();
    _motor2.hold();
}

void GantryMotor::update() {
    _motor1.update();

    // Motor 2 follows motor 1 with a position compensation to correct gantry skew
    // only if the motor 1 is in a position hold control mode
    pbio_control_type_t state = _motor1.getActuationStatus();
    if (state == PBIO_CONTROL_ANGLE || state == PBIO_CONTROL_TIMED) {
        // Apply a feedforward target to motor 2 based on motor 1 position and speed
        float speed1 = _motor1.speed();
        float angle1 = _motor1.angle();
        float angle2 = angle1 + (speed1 * (float)PBIO_CONFIG_SERVO_PERIOD_MS / 1000.0f / 2.0f); // Apply half the speed as feedforward
        angle2 += _skew_pos_compensation;
        _motor2.track_target(angle2);
    }
    else {
        // Motor 1 is not in position control mode, so just release motor 2
        _motor2.stop();
    }

    _motor2.update();
}
