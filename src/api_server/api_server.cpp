#include "api_server/api_server.hpp"
#include <LittleFS.h>

String RemoveQueryString(const String& uri) {
    int queryIndex = uri.indexOf('?');
    if (queryIndex != -1) {
        return uri.substring(0, queryIndex);
    }
    return uri;
}

String ApiRestServer::uriParam(const String& uri, uint8_t position) {
    uint8_t count = 0;
    int start = (uri[0] == '/') ? 1 : 0; // Skip leading '/'
    int end = uri.indexOf('/', start);
    
    while (end != -1) {
        if (count == position) {
            return RemoveQueryString(uri.substring(start, end));
        }
        count++;
        start = end + 1;
        end = uri.indexOf('/', start);
    }
    
    // Last segment (or only segment if no '/'
    if (count == position) {
        return RemoveQueryString(uri.substring(start));
    }
    
    return ""; // Return empty string if position is out of bounds
}

void ApiRestServer::begin(Settings* settings, WebFunctions* webFunctions, Motor* x1Motor, Motor* x2Motor) {
    _settings = settings;
    _webFunctions = webFunctions;
    _X1Motor = x1Motor;
    _X2Motor = x2Motor;

    // Confifure logging to Serial
    _loggingMid.setOutput(Serial);
    _server.addMiddleware(&_loggingMid);

    // CORS headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTION");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "rigin, X-Requested-With, Content-Type, Accept, Authorization");

    // Enable OPTION method for all endpoints
    _server.on("*", HTTP_OPTIONS, [](PsychicRequest *request, PsychicResponse *response) {
        return response->send(200);
    });

    // Setup API controllers
    setupSettingController();
    setupWebFunctionController();
    setupAxisLogController();
    setupAxisInfoController();

    // Serve assets static files from LittleFS removing the query string
    _server.on("/assets/*", [](PsychicRequest *request, PsychicResponse *response)
    {
        String url = request->url();
        String filename = RemoveQueryString(url);
        filename = "/www" + filename; // Prepend base path

        PsychicFileResponse response2(response, LittleFS, filename);

        return response2.send();
    });

    // Server other static files
    _server.serveStatic("/", LittleFS, "/www/");

    _server.begin();
}

Motor* ApiRestServer::getMotorByName(const char* name) {
    if (strcmp(name, "X1") == 0)
        return this->_X1Motor;
    else if (strcmp(name, "X2") == 0)
        return this->_X2Motor;
    else
        return nullptr;
}
