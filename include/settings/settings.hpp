#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "settings_group.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/motorwithreferenceswitch.hpp"
#include "motor_control/motorwithstallreference.hpp"
#include "axis/settings_axis_group.hpp"
#include "axis_stall_homing/settings_axis_stall_homing_group.hpp"

class Settings {
    private:
        MotorWithStallReference& _X1Motor;
        MotorWithStallReference& _X2Motor;

        SettingsAxisGroup _xSettings = SettingsAxisGroup("x_axis", "X Axis", _X1Motor, _X2Motor);
        SettingsAxisStallHomingGroup _xHomingSettings = SettingsAxisStallHomingGroup("x_axis_homing", "X Axis Homing", _X1Motor);

        SettingsGroup* _groups[2] = { &_xSettings, &_xHomingSettings };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(SettingsGroup*);
        
    public:
        Settings(MotorWithStallReference& x1Motor, MotorWithStallReference& x2Motor) 
            : _X1Motor(x1Motor), _X2Motor(x2Motor)
        {            
        }

        SettingsGroup* getGroup(const char* name);

        SettingsGroup** getGroups();
        uint16_t getGroupsCount() const;

        void storeInNVS();
        void restoreFromNVS();
};