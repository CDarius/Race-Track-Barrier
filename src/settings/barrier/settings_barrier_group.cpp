#include "settings/barrier/settings_barrier_group.hpp"

SettingsBarrierGroup::SettingsBarrierGroup(const char* name, const char* description,  barrier_config_t& config) : _config(config) 
{ 
    _name = name;
    _description = description;
}

const char* SettingsBarrierGroup::getName() const {
    return _name;
}

const char* SettingsBarrierGroup::getTitle() const {
    return _description;
}

ISetting** SettingsBarrierGroup::getSettings() {
    return _settings;
}
