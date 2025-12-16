#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/axis/web_function_axis_stepresponse.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"

class WebFunctionGroupAxis : public WebFunctionGroup {
    private:
        IMotorHoming& _motor;        
        WebFunctionAxisStepResponse _stepResponse = WebFunctionAxisStepResponse(_motor, _taskRunner);

        WebFunction* _functions[1] = {&_stepResponse};

    public:
        WebFunctionGroupAxis(const char* name, const char* title, TaskRunner& taskRunner, IMotorHoming& motor)
            : _motor(motor),  WebFunctionGroup(name, title, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
