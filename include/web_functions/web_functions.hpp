#pragma once

#include "web_function_group.hpp"
#include "axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "config.h"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner", OTHER_TASK_HIGH_PRIORITY);

        IMotorHoming& _X1Motor;
        IMotorHoming& _X2Motor;
        
        WebFunctionGroupAxis _x1AxisGroup = WebFunctionGroupAxis("x1_axis", "X1-Axis", _taskRunner, _X1Motor);
        WebFunctionGroupAxis _x2AxisGroup = WebFunctionGroupAxis("x2_axis", "X2-Axis", _taskRunner, _X2Motor);

        WebFunctionGroup* _groups[2] = {&_x1AxisGroup, &_x2AxisGroup };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(IMotorHoming& X1Motor, IMotorHoming& X2Motor)
            : _X1Motor(X1Motor), _X2Motor(X2Motor) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;

        // Returns true if any WebFunction in any group is InProgress
        bool checkAnyFunctionInProgress() const;
};
