#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>
#include "monotonic.h"
#include "config.h"
#include "api_server/api_server.hpp"
#include "devices/unit_encoder.hpp"
#include "devices/rgb_led.hpp"
#include "devices/button.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/gantrymotor.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/motor.hpp"
#include "motor_control/error.hpp"
#include "utils/i2c_utils.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

#include "io.h"
#include "manual_home.hpp"
#include "barrier_config.h"
#include "settings/settings.hpp"
#include "web_functions/web_functions.hpp"

barrier_config_t barrier_config = {
    .manual_homing_speed = 100.0f,
    .jog_multiplier = 5.0f
};

ApiRestServer server;

Motor x1_motor;
Motor x2_motor;
GantryMotor x_motor(x1_motor, x2_motor);

UnitEncoder knob_encoder;
RGBLed board_rgb_led;
Button start_button;
Button knob_button;
Adafruit_NeoPixel start_button_led = Adafruit_NeoPixel(1, START_BUTTON_LED_PIN, NEO_GRB + NEO_KHZ800);

ManualHome manual_home(knob_encoder, x_motor, start_button_led, barrier_config);

Settings game_settings(x_motor, barrier_config);
WebFunctions web_functions(x_motor, manual_home, knob_encoder, barrier_config);


bool service_mode = false;

void motor_loop_task(void *parameter) {
    int32_t counter = 0;
    bool led = false;

    while (true) {
        uint64_t start_time = monotonic_us();
        uint32_t millis = (uint32_t)(start_time / US_PER_MS);

        // Update motors motion
        x_motor.update();
        
        // Blink the watchdog LED every 150 iterations (450 ms)
        counter++;
        if (counter >= 150) {
            counter = 0;
            led = !led;
            digitalWrite(BOARD_LED_OUTPUT, led ? HIGH : LOW);
        }

        // Run the task every 3ms
        delay(PBIO_CONFIG_SERVO_PERIOD_MS);
    }
}

void setup_wifi() {
    // Connect to WiFi
    IPAddress staticIP(IP_ADDRESS);
    IPAddress gateway(IP_GATEWAY);
    IPAddress subnet(IP_SUBNET);
    IPAddress dns(IP_DNS);

#ifdef AP_MODE
    // Set up Access Point mode
    if (WiFi.softAPConfig(staticIP, gateway, subnet)) {
        if (WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
            board_rgb_led.setColor(RGB_COLOR_MAGENTA);
            Logger::instance().logI("Access Point started!");
            Logger::instance().logI("SSID: " + String(WIFI_SSID) + ", Password: " + String(WIFI_PASSWORD));
            Logger::instance().logI("AP IP Address: " + WiFi.softAPIP().toString());
        } else {
            Logger::instance().logE("Failed to start Access Point.");
            board_rgb_led.unrecoverableError();
        }
    } else {
        Logger::instance().logE("Access Point configuration failed. Please check the static IP settings.");
        board_rgb_led.unrecoverableError();
    }
#else
    // Connect to WiFi in Station mode
    if (WiFi.config(staticIP, gateway, subnet, dns, dns)) {
        board_rgb_led.setColor(RGB_COLOR_MAGENTA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Logger::instance().logI("Connecting to WiFi...");
        }
        Logger::instance().logI("Connected to WiFi!");
        Logger::instance().logI("IP Address: " + WiFi.localIP().toString());
    } 
    else {
        Logger::instance().logE("WiFi configuration failed. Please check the static IP settings.");
        board_rgb_led.unrecoverableError();
    }
#endif
}

void setup() {    
    // Configure USB uarts
    Serial.begin(115200);

    // Configure inputs
    pinMode(START_BUTTON_PIN, INPUT_PULLUP);

    // Configure outputs
    pinMode(BOARD_LED_OUTPUT, OUTPUT);
    board_rgb_led.begin();
    board_rgb_led.setColor(RGB_COLOR_WHITE);

    // Start button RGB LED
    start_button_led.begin();
    start_button_led.setBrightness(100); 
    start_button_led.setPixelColor(0, RGB_COLOR_RED);
    start_button_led.show();
    
    // Configure two I2C busses
    Wire.begin(I2C_BUS_1_SDA, I2C_BUS_1_SCL, 400000);
    Wire1.begin(I2C_BUS_2_SDA, I2C_BUS_2_SCL, 400000);
    knob_encoder.begin(&Wire);
    knob_encoder.setPulseMode();
    knob_encoder.setLEDColor(0, RGB_COLOR_BLACK);
    knob_encoder.setLEDColor(1, RGB_COLOR_BLACK);

    x1_motor.begin("X1", X1_AXIS_ENC_PIN_1, X1_AXIS_ENC_PIN_2, X1_AXIS_PWM_PIN_1, X1_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    x2_motor.begin("X2", X2_AXIS_ENC_PIN_1, X2_AXIS_ENC_PIN_2, X2_AXIS_PWM_PIN_1, X2_AXIS_PWM_PIN_2, PBIO_DIRECTION_COUNTERCLOCKWISE, 1.0, &settings_servo_ev3_large);
    x_motor.begin("x");

    // Restore game and axes settings from NVS
    game_settings.restoreFromNVS();
    
    service_mode = START_BUTTON_PRESSED;
    bool start_web_server = service_mode;
#ifdef FORCE_SERVICE_MODE
    service_mode = true;
    start_web_server = true;
#endif
#ifdef FORCE_START_WEB_SERVER
    start_web_server = true;
#endif

    // Start motor loop task on core 0. Core 0 is used only for motor control
    xTaskCreatePinnedToCore (
        motor_loop_task,        // Function to implement the task
        "motor_loop",           // Name of the task
        8000,                   // Stack size in words
        NULL,                   // Task input parameter
        MOTION_TASK_PRIORITY,   // Priority of the task
        NULL,                   // Task handle.
        MOTION_TASK_CORE        // Core where the task should run
    );
    delay(100); // Ensure that motor loop task is running

    if (start_web_server) {
        setup_wifi();

        // Mount LittleFS
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS");
            board_rgb_led.unrecoverableError();
        }
        Serial.println("LittleFS mounted successfully");

        // Start the web server
        server.begin(
            &game_settings, &web_functions, 
            &x1_motor, &x2_motor);
    }

    // Service mode infinite loop to prevent normal operation
    if (service_mode) {
        board_rgb_led.setColor(RGB_COLOR_BLUE);
        while (true)
        {
            delay(1000);
        }        
    }

    // Run manual homing procedure
    board_rgb_led.setColor(RGB_COLOR_YELLOW);
    CancelToken cancel_token;
    manual_home.run_home(cancel_token);

    // Set board LED to green to indicate normal operation
    board_rgb_led.setColor(RGB_COLOR_GREEN);
}

void loop() {
    // Wait for the start button to be released
    while (START_BUTTON_PRESSED)
        delay(50);

    // Raise the barrier
    start_button_led.setPixelColor(0, RGB_COLOR_YELLOW);
    start_button_led.show();
    x_motor.set_actuation_limit(barrier_config.barrier_raise_power);
    x_motor.run_target(barrier_config.barrier_raise_speed, x_motor.getSwLimitPlus(), PBIO_ACTUATION_HOLD, true);
    x_motor.set_actuation_limit(100);

    // Indicate readiness by setting the start button LED to green
    start_button_led.setPixelColor(0, RGB_COLOR_GREEN);
    start_button_led.show();

    // Wait for the start button to be clicked
    while (!START_BUTTON_PRESSED)
        delay(50);

    // Start lowering the barrier
    start_button_led.setPixelColor(0, RGB_COLOR_RED);
    start_button_led.show();
    x_motor.run_target(barrier_config.barrier_lower_speed, x_motor.getSwLimitMinus(), PBIO_ACTUATION_HOLD, true);

    // Wait with the barrier lowered
    bool led = true;
    uint32_t raise_time = millis() + (barrier_config.barrier_hold_time * 1000);
    while (millis() < raise_time) {
        // Blink the start button LED
        led = !led;
        start_button_led.setPixelColor(0, led ? RGB_COLOR_RED : RGB_COLOR_BLACK);
        start_button_led.show();
        delay(300);
    }
}