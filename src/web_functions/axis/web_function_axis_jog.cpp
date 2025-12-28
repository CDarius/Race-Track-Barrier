#include "web_functions/axis/web_function_axis_jog.hpp"

const char* WebFunctionAxisJog::getName() const {
    return "jog";
}

const char* WebFunctionAxisJog::getTitle() const {
    return "Jog axis";
}

const char* WebFunctionAxisJog::getDescription() const {
    return "Jog a single axis";
}

uint16_t WebFunctionAxisJog::getPrerequisitesCount() const {
    return 0; // No prerequisites
}

const char* WebFunctionAxisJog::getPrerequisiteDescription(uint16_t index) const {
    return nullptr;
}

void WebFunctionAxisJog::arePrerequisitesMet(bool* results) const {
    // No prerequisites, so nothing to check
}

WebFunctionExecutionStatus WebFunctionAxisJog::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis jog asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxisJog* self = static_cast<WebFunctionAxisJog*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        int16_t knob_value = INT16_MIN;
        float current_angle = self->_motor.angle();
        float angle_setpoint = current_angle;

        // Jog loop until the user requests to stop
        while(!self->_cancelToken->isCancelled()) {
            // Update knob delta value
            int16_t new_knob_value;
            int16_t knob_delta = 0;
            if (self->_knob_encoder.getValue(new_knob_value)) {
                new_knob_value /= 2; // One count per encoder detent
                if (knob_value == INT16_MIN) {
                    knob_value = new_knob_value;
                } else {
                    knob_delta = new_knob_value - knob_value;
                    knob_value = new_knob_value;
                }
            } else {
                knob_value = INT16_MIN;
            }

            // Jog the barrier up/down with the knob rotation
            if (knob_delta != 0) {
                angle_setpoint += (float)knob_delta * self->barrier_config.jog_multiplier;
                self->_motor.track_target(angle_setpoint);
            }

            // Display current angle if changed
            float new_angle = self->_motor.angle();
            if (new_angle != current_angle) {
                current_angle = new_angle;
                Serial.printf("Jogging to angle: %.2f deg\n", current_angle);
            }
            
            delay(PBIO_CONFIG_SERVO_PERIOD_MS * 10); // Sleep the task
        }
        self->_status = WebFunctionExecutionStatus::Done;

        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxisJog::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
