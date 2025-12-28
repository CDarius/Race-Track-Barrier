#pragma once

#include <Arduino.h>
#include "web_functions/web_function.hpp"
#include "motor_control/gantrymotor.hpp"
#include "barrier_config.h"
#include "devices/unit_encoder.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"

class WebFunctionAxisJog : public WebFunction {
    private:
        GantryMotor& _motor;
        UnitEncoder& _knob_encoder;
        const barrier_config_t& barrier_config;
        TaskRunner& _taskRunner;
        CancelToken* _cancelToken = nullptr;

    public:
        WebFunctionAxisJog(GantryMotor& motor, UnitEncoder& knob_encoder, const barrier_config_t& barrier_config, TaskRunner& taskRunner)
            : _motor(motor), _knob_encoder(knob_encoder), barrier_config(barrier_config), _taskRunner(taskRunner) {};

        // Override methods as needed
        const char* getName() const override;
        const char* getTitle() const override;
        const char* getDescription() const override;
        uint16_t getPrerequisitesCount() const override;
        const char* getPrerequisiteDescription(uint16_t index) const override;

        void arePrerequisitesMet(bool* results) const override;
        WebFunctionExecutionStatus start() override;
        void stop() override;
};
