#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "manual_home.hpp"
#include "motor_control/gantrymotor.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"

class WebFunctionAxisHoming : public WebFunction{
private:
    GantryMotor& _axis;
    ManualHome& _manualHome;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;

public:
    // Constructor that takes and stores an GantryMotor reference
    WebFunctionAxisHoming(GantryMotor& axis, ManualHome& manualHome, TaskRunner& taskRunner) : 
        _axis(axis), _manualHome(manualHome), _taskRunner(taskRunner) {};

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
