#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axis_swlimitm.hpp"
#include "setting_axis_swlimitp.hpp"
#include "setting_axis_maxspeed.hpp"
#include "setting_axis_maxacc.hpp"
#include "setting_axis_postolerance.hpp"
#include "settings/axis/setting_axis_pidkp.hpp"
#include "settings/axis/setting_axis_pidki.hpp"
#include "settings/axis/setting_axis_pidkd.hpp"
#include "settings/axis/setting_axis_integralrange.hpp"
#include "settings/axis/setting_axis_integralrate.hpp"
#include "settings/axis/setting_axis_maxwindupfactor.hpp"
#include "motor_control\motor.hpp"

class SettingsAxisGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _title;

        Motor& _motor1;
        Motor& _motor2;
        AxisSwLimitMSetting _swLimitM = AxisSwLimitMSetting(_motor1, _motor2);
        AxisSwLimitPSetting _swLimitP = AxisSwLimitPSetting(_motor1, _motor2);
        AxisMaxSpeedSetting _maxSpeed = AxisMaxSpeedSetting(_motor1, _motor2);
        AxisMaxAccelerationSetting _maxAcc = AxisMaxAccelerationSetting(_motor1, _motor2);
        AxisPosToleranceSetting _posTolerance = AxisPosToleranceSetting(_motor1, _motor2);
        AxisPidKpSetting _pidKp = AxisPidKpSetting(_motor1, _motor2);
        AxisPidKiSetting _pidKi = AxisPidKiSetting(_motor1, _motor2);
        AxisPidKdSetting _pidKd = AxisPidKdSetting(_motor1, _motor2);
        AxisIntegralRangeSetting _integralRange = AxisIntegralRangeSetting(_motor1, _motor2);
        AxisIntegralRateSetting _integralRate = AxisIntegralRateSetting(_motor1, _motor2);
        AxisMaxWindupFactorSetting _maxWindupFactor = AxisMaxWindupFactorSetting(_motor1, _motor2);

        ISetting* _settings[11] = {
            &_swLimitM, &_swLimitP, 
            &_maxSpeed, &_maxAcc, &_posTolerance, 
            &_pidKp, &_pidKi, &_pidKd,
            &_integralRange, &_integralRate, 
            &_maxWindupFactor
        };

    public:
        SettingsAxisGroup(const char* name, const char* title, Motor& motor1, Motor& motor2);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};