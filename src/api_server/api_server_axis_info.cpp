#include "api_server/api_server.hpp"

void ApiRestServer::setupAxisInfoController() {
    // Get axis info
    _server.on("/axesinfo", [this](PsychicRequest *request, PsychicResponse *response)
    {
        // Prepare JSON response
        JsonDocument doc;
        JsonArray jAxes = doc.to<JsonArray>();
        for(uint8_t i = 0; i < 2; i++) {
            Motor* motor = nullptr;
            switch(i) {
                case 0: motor = _X1Motor; break;
                case 1: motor = _X2Motor; break;
            }
            if (motor) {
                float axis_speed_tolerance, axis_position_tolerance;
                motor->get_target_tolerances(&axis_speed_tolerance, &axis_position_tolerance);

                JsonObject jAxis = jAxes.add<JsonObject>();
                jAxis["name"] = motor->name();
                jAxis["counts_per_unit"] = motor->get_counts_per_unit();      
                jAxis["standstill_speed"] = axis_speed_tolerance;
                jAxis["position_tolerance"] = axis_position_tolerance;    
                jAxis["speed_limit"] = motor->get_speed_limit();
                jAxis["acceleration_limit"] = motor->get_acceleration_limit();
                jAxis["actuation_limit"] = motor->get_actuation_limit();
                jAxis["sw_limit_m"] = motor->getSwLimitMinus();
                jAxis["sw_limit_p"] = motor->getSwLimitPlus();
            }
        }

        String responseStr;
        serializeJson(doc, responseStr);
        return response->send(200, "application/json", responseStr.c_str());
    });
}