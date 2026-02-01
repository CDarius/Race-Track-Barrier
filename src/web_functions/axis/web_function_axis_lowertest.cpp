#include "web_functions/axis/web_function_axis_lowertest.hpp"

const char* WebFunctionAxisLowerTest::getName() const {
    return "axis_lowertest";
}

const char* WebFunctionAxisLowerTest::getTitle() const {
    return "Axis Lower Test";
}

const char* WebFunctionAxisLowerTest::getDescription() const {
    return "Lower the axis and log the response";
}

uint16_t WebFunctionAxisLowerTest::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionAxisLowerTest::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionAxisLowerTest::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionAxisLowerTest::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis step log asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxisLowerTest* self = static_cast<WebFunctionAxisLowerTest*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        float sw_limit_plus = self->_axis.getSwLimitPlus();
        float sw_limit_minus = self->_axis.getSwLimitMinus();

        // Move the axis at plus sw limit
        pbio_error_t err = self->_axis.run_target(
            self->_axis.get_speed_limit() / 4.0, // Use 1/4 of max speed
            sw_limit_plus,
            PBIO_ACTUATION_HOLD,
            true,
            &cancel_token);

        IF_CANCELLED(cancel_token, {
            self->_status = WebFunctionExecutionStatus::Done;
            self->_cancelToken = nullptr;
            return;
        });

        if (err != PBIO_SUCCESS) {
            Logger::instance().logE("Error during step 1 " + String(self->_axis.name()) + "-axis lower test: " + String(pbio_error_str(err)));
            self->_failureDescription = "Failed to reach start position (sw limit +)";
            self->_status = WebFunctionExecutionStatus::Failed;
            self->_cancelToken = nullptr;
            return;
        }

        // Let the axis settle and then kill the PWM command
        delay(1000); // Let the axis settle
        self->_axis.stop(); // Ensure the motor is stopped
        delay(20);

        // Start axis log and give a step move command towards plus sw limit
        self->_axis.get_logger()->start(RUN_STEP_RESPONSE_LOG_DURATION_MS, 1);
        delay(100); // Wait a bit to ensure the logger is started
        float target_position = sw_limit_minus;
        self->_axis.run_target(
            self->_axis.get_speed_limit(),
            target_position,
            PBIO_ACTUATION_HOLD,
            false,
            &cancel_token);
        
        // Wait for the axis to reach the target position
        float axis_speed_tolerance, axis_position_tolerance;
        self->_axis.get_target_tolerances(&axis_speed_tolerance, &axis_position_tolerance);
        unsigned long begin_on_target_time = 0;
        while(true) {
            IF_CANCELLED(cancel_token, {
                self->_status = WebFunctionExecutionStatus::Done;
                self->_cancelToken = nullptr;
                return;
            });

            bool on_target = fabs(self->_axis.angle() - target_position) < axis_position_tolerance &&
                fabs(self->_axis.speed()) < axis_speed_tolerance;

            if (on_target) {
                if (begin_on_target_time == 0) {
                    begin_on_target_time = millis();
                } else if ((millis() - begin_on_target_time) >= RUN_STEP_RESPONSE_SETTLE_TIME_MS) {
                    // Axis is on target for the required time
                    break;
                }
            } else {
                begin_on_target_time = 0;
            }

            delay(PBIO_CONFIG_SERVO_PERIOD_MS * 10);
        }

        // Stop the log and release the axis
        self->_axis.get_logger()->stop();
        self->_axis.stop();

        self->_status = WebFunctionExecutionStatus::Done;
        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxisLowerTest::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
