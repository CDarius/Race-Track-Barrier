#pragma once

#include <Arduino.h>
#include "utils/task_runner.hpp"
#include "web_function.hpp"

class WebFunctionGroup {
    protected:
        const char* _name;
        const char* _title;
        TaskRunner& _taskRunner;

        WebFunctionGroup(const char* name, const char* title, TaskRunner& taskRunner)
            : _name(name), _title(title), _taskRunner(taskRunner) {}

    public:
        const char* getName() const {
            return _name;
        }

        const char* getTitle() const {
            return _title;
        }

        virtual WebFunction** getFunctions() = 0;
        virtual uint16_t getFunctionsCount() const = 0;

        WebFunction* getFunction(const char* name);
};
