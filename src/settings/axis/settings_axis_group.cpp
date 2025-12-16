#include "settings/axis/settings_axis_group.hpp"

SettingsAxisGroup::SettingsAxisGroup(const char* name, const char* description, Motor& motor1, Motor& motor2) : _motor1(motor1), _motor2(motor2) 
{ 
    _name = name;
    _title = description;
}

//virtual const char* getName() const;
const char* SettingsAxisGroup::getName() const {
    return _name;
}

const char* SettingsAxisGroup::getTitle() const {
    return _title;
}

ISetting** SettingsAxisGroup::getSettings() {
    return _settings;
}
