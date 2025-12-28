#pragma once

#include "web_function_group.hpp"
#include "axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "motor_control/gantrymotor.hpp"
#include "manual_home.hpp"
#include "barrier_config.h"
#include "devices/unit_encoder.hpp"
#include "config.h"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner", OTHER_TASK_HIGH_PRIORITY);

        GantryMotor& _XMotor;
        ManualHome& _manualHome;
        UnitEncoder& _knob_encoder;
        const barrier_config_t& barrier_config;
        
        WebFunctionGroupAxis _xAxisGroup = WebFunctionGroupAxis("x_axis", "X-Axis", _taskRunner, _XMotor, _manualHome, _knob_encoder, barrier_config);

        WebFunctionGroup* _groups[1] = {&_xAxisGroup };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(GantryMotor& xMotor, ManualHome& manualHome, UnitEncoder& knob_encoder, const barrier_config_t& barrier_config)
            : _XMotor(xMotor), _manualHome(manualHome), _knob_encoder(knob_encoder), barrier_config(barrier_config) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;

        // Returns true if any WebFunction in any group is InProgress
        bool checkAnyFunctionInProgress() const;
};
