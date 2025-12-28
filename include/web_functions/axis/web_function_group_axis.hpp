#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/axis/web_function_axis_homing.hpp"
#include "web_functions/axis/web_function_axis_jog.hpp"
#include "web_functions/axis/web_function_axis_stepresponse.hpp"
#include "motor_control/gantrymotor.hpp"
#include "manual_home.hpp"
#include "barrier_config.h"
#include "devices/unit_encoder.hpp"
#include "utils/task_runner.hpp"

class WebFunctionGroupAxis : public WebFunctionGroup {
    private:
        GantryMotor& _motor;
        ManualHome& _manualHome;
        UnitEncoder& _knob_encoder;
        const barrier_config_t& barrier_config;

        WebFunctionAxisHoming _homing = WebFunctionAxisHoming(_motor, _manualHome, _taskRunner);
        WebFunctionAxisJog _jog = WebFunctionAxisJog(_motor, _knob_encoder, barrier_config, _taskRunner);
        WebFunctionAxisStepResponse _stepResponse = WebFunctionAxisStepResponse(_motor, _taskRunner);

        WebFunction* _functions[3] = { &_homing, &_jog, &_stepResponse};

    public:
        WebFunctionGroupAxis(const char* name, const char* title, TaskRunner& taskRunner, 
            GantryMotor& motor, ManualHome& manualHome, UnitEncoder& knob_encoder, const barrier_config_t& barrierConfig)
            : _motor(motor), _manualHome(manualHome), _knob_encoder(knob_encoder), barrier_config(barrierConfig), WebFunctionGroup(name, title, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
