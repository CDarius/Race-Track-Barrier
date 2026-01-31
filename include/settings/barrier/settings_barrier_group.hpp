#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_barrier_homing_speed.hpp"
#include "setting_barrier_homing_jog_multiplier.hpp"
#include "barrier_config.h"
#include "setting_barrier_lowering_speed.hpp"
#include "setting_barrier_raising_speed.hpp"
#include "setting_barrier_raising_power.hpp"
#include "setting_barrier_hold_time.hpp"

class SettingsBarrierGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _description;

        barrier_config_t& _config;

        BarrierHomingSpeedSetting _speed = BarrierHomingSpeedSetting(_config);
        BarrierJogMultiplierSetting _jog_multiplier = BarrierJogMultiplierSetting(_config);
        BarrierLoweringSpeedSetting _lowering_speed = BarrierLoweringSpeedSetting(_config);
        BarrierRaisingSpeedSetting _raising_speed = BarrierRaisingSpeedSetting(_config);
        BarrierRaisingPowerSetting _raising_power = BarrierRaisingPowerSetting(_config);
        BarrierHoldTimeSetting _hold_time = BarrierHoldTimeSetting(_config);

        ISetting* _settings[6] = {
            &_speed, &_jog_multiplier,
            &_lowering_speed, &_raising_speed, &_raising_power, 
            &_hold_time};

    public:
        SettingsBarrierGroup(const char* name, const char* description, barrier_config_t& config);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};