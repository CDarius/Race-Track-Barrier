#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "settings_group.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/gantrymotor.hpp"
#include "barrier_config.h"
#include "axis/settings_axis_group.hpp"
#include "barrier/settings_barrier_group.hpp"

class Settings {
    private:
        GantryMotor& _XMotor;
        Motor& _X1Motor;
        Motor& _X2Motor;
        barrier_config_t& _barrierConfig;

        SettingsAxisGroup _xSettings = SettingsAxisGroup("x_axis", "X Axis", _X1Motor, _X2Motor);
        SettingsBarrierGroup _barrierSettings = SettingsBarrierGroup("barrier", "Barrier Settings", _barrierConfig);

        SettingsGroup* _groups[2] = { &_xSettings, &_barrierSettings };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(SettingsGroup*);
        
    public:
        Settings(GantryMotor& xMotor, barrier_config_t& barrierConfig) 
            : _XMotor(xMotor), _X1Motor(xMotor.motor1()), _X2Motor(xMotor.motor2()), _barrierConfig(barrierConfig)
        {            
        }

        SettingsGroup* getGroup(const char* name);

        SettingsGroup** getGroups();
        uint16_t getGroupsCount() const;

        void storeInNVS();
        void restoreFromNVS();
};