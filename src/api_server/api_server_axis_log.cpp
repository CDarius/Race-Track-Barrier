#include "api_server/api_server.hpp"

void ApiRestServer::setupAxisLogController() {
    // Start logging
    _server.on("/axislog/start/*", [this](PsychicRequest *request, PsychicResponse *response)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        PBIOLogger* logger = getMotorLoggerByName(axis.c_str());
        if (!logger)
            return response->send(400);
        
        // Get optional parameters
        uint32_t duration = 10 * 1000; // default 10s
        uint32_t div = 1; // default log every sample
        if (request->hasParam("duration")) {
            duration = request->getParam("duration")->value().toInt() * 1000; // convert s to ms
            if (duration < 1)
                return response->send(400);

        }
        if (request->hasParam("div")) {
            div = request->getParam("div")->value().toInt();
            if (div < 1)
                return response->send(400);
        }

        // Start the log
        pbio_error_t err = logger->start(duration, div);
        if (err != PBIO_SUCCESS)
            return response->send(500);

        return response->send(200);
    });

    // Stop logging
    _server.on("/axislog/stop/*", [this](PsychicRequest *request, PsychicResponse *response)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        PBIOLogger* logger = getMotorLoggerByName(axis.c_str());
        if (!logger)
            return response->send(400);

        logger->stop();
        return response->send(200);
    });

    // Read the log
    _server.on("/axislog/read/*", [this](PsychicRequest *request, PsychicResponse *response)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        PBIOLogger* logger = getMotorLoggerByName(axis.c_str());
        if (!logger)
            return response->send(400);

        int32_t rows = logger->rows();
        int32_t cols = logger->cols();

        if (rows == 0 || cols == 0)
            return response->send(204); // No content

        // Start streaming response
        PsychicStreamResponse response2(response, "application/json");
        response2.beginSend();

        // Write JSON header
        response2.print("{\"rows\":");
        response2.print(String(rows));
        response2.print(",\"cols\":");
        response2.print(String(cols));
        yield(); // Allow background tasks to run

        // Write column names
        response2.print(",\"col_names\":[");
        for (uint32_t c = 0; c < cols; c++) {
            const char* col_name = logger->col_name(c);
            if (col_name) {
                response2.print("\"");
                response2.print(col_name);
                response2.print("\"");
            }
            else {
                response2.print("null");
            }
            if (c < cols - 1) {
                response2.print(",");
            }
        }
        response2.print("]");
        yield(); // Allow background tasks to run

        // Write column units
        response2.print(",\"col_units\":[");
        for (uint32_t c = 0; c < cols; c++) {
            const char* col_unit = logger->col_unit(c);
            if (col_unit) {
                response2.print("\"");
                response2.print(col_unit);
                response2.print("\"");
            }
            else {
                response2.print("null");
            }
            if (c < cols - 1) {
                response2.print(",");
            }
        }
        response2.print("]");
        yield(); // Allow background tasks to run

        // Write the data array
        response2.print(",\"data\":[");
        int32_t row_buffer[cols];
        int8_t yield_counter = 0;
        for (uint32_t r = 0; r < rows; r++) {
            // Get a log row
            pbio_error_t result = logger->read(r, row_buffer);
            if (result != PBIO_SUCCESS) {
                response2.endSend();
                return response->send(500);
            }

            // Write the row as a JSON array
            response2.print("[");
            for (uint32_t c = 0; c < cols; c++) {
                int32_t value = row_buffer[c];
                response2.print(value);
                if (c < cols - 1) {
                    response2.print(",");
                }
            }
            response2.print("]");
            if (r < rows - 1) {
                response2.print(",");
            }

            // Yield every 10 rows to allow background tasks to run
            if (++yield_counter >= 10) {
                yield_counter = 0;
                yield(); // Allow background tasks to run
            }
        }
        response2.print("]}");

        return response2.endSend();
    });

    // Return log running status
    _server.on("/axislog/running/*", [this](PsychicRequest *request, PsychicResponse *response) {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        PBIOLogger* logger = getMotorLoggerByName(axis.c_str());
        if (!logger)
            return response->send(400);
        
        // Prepare the answer
        JsonDocument doc;
        doc["axis"] = axis;
        doc["running"] = logger->is_active();

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return response->send(200, "application/json", jsonResponse.c_str());
    });
}
