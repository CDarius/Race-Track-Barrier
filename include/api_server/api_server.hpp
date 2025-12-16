#pragma once

#include <PsychicHttp.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings/settings.hpp"
#include "settings/settings_group.hpp"
#include "settings/setting.hpp"
#include "web_functions/web_functions.hpp"
#include "motor_control/motor.hpp"

class ApiRestServer {
    private:
        PsychicHttpServer _server;
        LoggingMiddleware _loggingMid;
        Settings* _settings;
        WebFunctions* _webFunctions;
        Motor* _X1Motor;
        Motor* _X2Motor;


        String uriParam(const String& uri, uint8_t position);
        void setupSettingController();
        void setupWebFunctionController();
        void setupAxisLogController();
        void setupAxisInfoController();

        Motor* getMotorByName(const char* name);

    public:
        void begin(Settings* settings, WebFunctions* webFunctions, Motor* x1Motor, Motor* x2Motor);
};
