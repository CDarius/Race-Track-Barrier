#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_barrier_homing_speed.hpp"
#include "setting_barrier_homing_jog_multiplier.hpp"
#include "barrier_config.h"

class SettingsBarrierGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _description;

        barrier_config_t& _config;
        BarrierHomingSpeedSetting _speed = BarrierHomingSpeedSetting(_config);
        BarrierJogMultiplierSetting _jog_multiplier = BarrierJogMultiplierSetting(_config);

        ISetting* _settings[2] = {&_speed, &_jog_multiplier};

    public:
        SettingsBarrierGroup(const char* name, const char* description, barrier_config_t& config);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};