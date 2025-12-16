#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>
#include "monotonic.h"
#include "config.h"
#include "api_server/api_server.hpp"
#include "devices/rgb_led.hpp"
#include "devices/button.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/motorwithreferenceswitch.hpp"
#include "motor_control/motorwithstallreference.hpp"
#include "motor_control/error.hpp"
#include "utils/i2c_utils.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

#include "settings/settings.hpp"
#include "web_functions/web_functions.hpp"

// ****************************************************
// *** I/O Pins
// ****************************************************

#define X1_AXIS_PWM_PIN_1        13
#define X1_AXIS_PWM_PIN_2        12
#define X1_AXIS_ENC_PIN_1        11
#define X1_AXIS_ENC_PIN_2        10

#define X2_AXIS_PWM_PIN_1        9
#define X2_AXIS_PWM_PIN_2        46
#define X2_AXIS_ENC_PIN_1        3
#define X2_AXIS_ENC_PIN_2        8

#define L_AXIS_PWM_PIN_1        18
#define L_AXIS_PWM_PIN_2        17
#define L_AXIS_ENC_PIN_1        16
#define L_AXIS_ENC_PIN_2        15

#define R_AXIS_PWM_PIN_1        4
#define R_AXIS_PWM_PIN_2        5
#define R_AXIS_ENC_PIN_1        6
#define R_AXIS_ENC_PIN_2        7

#define IO_BOARD_UART_RX        39
#define IO_BOARD_UART_TX        40

#define I2C_BUS_1_SDA           2
#define I2C_BUS_1_SCL           1

#define I2C_BUS_2_SDA           41
#define I2C_BUS_2_SCL           42

#define BOARD_LED_OUTPUT        38
#define START_BUTTON_PIN        43
#define START_BUTTON_LED_PIN    44

#define START_BUTTON_PRESSED    (digitalRead(START_BUTTON_PIN) == LOW)

// ****************************************************

ApiRestServer server;

gantry_stall_homing_config_t x_motor_homing_config = {
    .skew_pos_compensation = 0.0 // No skew compensation
};

MotorWithStallReference x1_motor(x_motor_homing_config);
MotorWithStallReference x2_motor(x_motor_homing_config);

RGBLed board_rgb_led;
Button start_button;
Adafruit_NeoPixel start_button_led = Adafruit_NeoPixel(1, START_BUTTON_LED_PIN, NEO_GRB + NEO_KHZ800);

Settings game_settings(x1_motor, x2_motor);
WebFunctions web_functions(x1_motor, x2_motor);

bool service_mode = false;

void motor_loop_task(void *parameter) {
    int32_t counter = 0;
    bool led = false;

    while (true) {
        uint64_t start_time = monotonic_us();
        uint32_t millis = (uint32_t)(start_time / US_PER_MS);

        // Update stop buttons status
        start_button.setRawState(millis, START_BUTTON_PRESSED);

        // Update motors motion
        x1_motor.update();
        x2_motor.update();
        
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

bool start_zero_axes_tasks = false;
SemaphoreHandle_t zero_axes_semaphore;

void zero_axes_task(void *parameter) {
    // Cast the parameter to a MotorWithStallReference pointer
    MotorWithStallReference *motor = static_cast<MotorWithStallReference *>(parameter);
    stall_homing_config_t* config = motor->config();
    float forward_speed = config->speed;
    float backward_speed = -2.0 * forward_speed;
    float backward_final_speed = motor->get_speed_limit() * (forward_speed > 0 ? -1 : 1);

    while (true) {
        // Wait until the main setup signals to start zeroing
        while (!start_zero_axes_tasks) {
            delay(50);
        }

        // Move the axes until stalled to find the home position
        motor->run_until_stalled(forward_speed, config->duty_limit, PBIO_ACTUATION_BRAKE);
        delay(500);

        // Signal that the task has completed its movement
        xSemaphoreGive(zero_axes_semaphore);

        // Wait until the start zeroing signal is cleared
        while (start_zero_axes_tasks) {
            delay(50);
        }
    }        
}

void home_all_axes() {
    gantry_stall_homing_config_t* config = (gantry_stall_homing_config_t*)x1_motor.config();
    float forward_speed = config->speed;
    float backward_speed = x1_motor.get_speed_limit() * (forward_speed > 0 ? -1 : 1);
    float backward_distance = config->minimum_travel * 1.2;

    while (true) {
        float x1_start_position = x1_motor.angle();
        float x2_start_position = x2_motor.angle();

        // Start zeroing of the two axes and wait for their completion
        zero_axes_semaphore = xSemaphoreCreateCounting(2, 0); // Counting semaphore for two tasks
        start_zero_axes_tasks = true;
        for (int i = 0; i < 2; i++) {
            if (xSemaphoreTake(zero_axes_semaphore, portMAX_DELAY) == pdTRUE) {
                Serial.println("A zero_axes_task has completed.");
            }
        }
        delay(1000);
        Serial.println("Both zero_axes_task tasks have completed.");
        start_zero_axes_tasks = false;

        // Calculate the movement deltas
        float x1_delta = abs(x1_motor.angle() - x1_start_position);
        float x2_delta = abs(x2_motor.angle() - x2_start_position);

        // Check if the movement deltas are acceptable
        if (x1_delta >= config->minimum_travel && x2_delta >= config->minimum_travel) {
            x1_motor.reset_angle(config->axis_position_at_home_marker);
            x2_motor.reset_angle(config->axis_position_at_home_marker - config->skew_pos_compensation);
            Serial.println("Both axes zeroed successfully.");
            break;
        }

        // Retract both axes and try again
        Serial.println("One or both axes did not move the minimum required distance. Retracting and retrying...");
        x1_motor.run_angle(backward_speed, backward_distance, PBIO_ACTUATION_BRAKE, false);
        x2_motor.run_angle(backward_speed, backward_distance, PBIO_ACTUATION_BRAKE, false);
        delay(100);
        while (!x1_motor.is_completion() || !x2_motor.is_completion()) {
            delay(50);
        }
    }

    // Release the axes
    x1_motor.stop();
    x2_motor.stop();
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
    // Wire.begin(I2C_BUS_1_SDA, I2C_BUS_1_SCL, 400000);
    // Wire1.begin(I2C_BUS_2_SDA, I2C_BUS_2_SCL, 400000);

    // Configure motors
    x_motor_homing_config.start_in_positive_direction = false,
    x_motor_homing_config.speed = 60.0, // Speed in motor degrees/second
    x_motor_homing_config.duty_limit = 50.0, // Duty cycle limit to detect stall on the reference obstacle (0.0% to 100.0%)
    x_motor_homing_config.minimum_travel = 90.0, // Minimum travel distance before hitting the switch in degrees

    x1_motor.begin("X1", X1_AXIS_ENC_PIN_1, X1_AXIS_ENC_PIN_2, X1_AXIS_PWM_PIN_1, X1_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    x2_motor.begin("X2", X2_AXIS_ENC_PIN_1, X2_AXIS_ENC_PIN_2, X2_AXIS_PWM_PIN_1, X2_AXIS_PWM_PIN_2, PBIO_DIRECTION_COUNTERCLOCKWISE, 1.0, &settings_servo_ev3_large);

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

    // Create two tasks on core OTHER_TASK_CORE
    xTaskCreatePinnedToCore(
        zero_axes_task,        // Function to implement the task
        "zero_axes_task_1",   // Name of the task
        2000,                  // Stack size in words
        &x1_motor,             // Task input parameter
        OTHER_TASK_PRIORITY,   // Priority of the task
        NULL,                  // Task handle
        OTHER_TASK_CORE        // Core where the task should run
    );

    xTaskCreatePinnedToCore(
        zero_axes_task,        // Function to implement the task
        "zero_axes_task_2",   // Name of the task
        2000,                  // Stack size in words
        &x2_motor,             // Task input parameter
        OTHER_TASK_PRIORITY,   // Priority of the task
        NULL,                  // Task handle
        OTHER_TASK_CORE        // Core where the task should run
    );

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

    board_rgb_led.setColor(RGB_COLOR_YELLOW);

    home_all_axes();

    board_rgb_led.setColor(RGB_COLOR_GREEN);
}

void move_all_axes(float speed, float postion) {
    x1_motor.run_target(speed, postion, PBIO_ACTUATION_HOLD, false);
    x2_motor.run_target(speed, postion, PBIO_ACTUATION_HOLD, false);
    delay(100);
    while (!x1_motor.is_completion() || !x2_motor.is_completion()) {
        delay(50);
    }

    // Wait a bit to ensure the axes are fully stopped and the release the axes
    delay(100);
    x1_motor.stop();
    x2_motor.stop();
}

void loop() {
    // Move barrier up
    x1_motor.set_actuation_limit(70);
    x2_motor.set_actuation_limit(70);
    move_all_axes(x1_motor.get_speed_limit(), x1_motor.getSwLimitPlus());

    // set start button LED to green
    start_button_led.setPixelColor(0, RGB_COLOR_GREEN);
    start_button_led.show();

    // Wait for the start button press and release
    while (start_button.isPressed()) { delay(50);}
    while (!start_button.isPressed()) { delay(50); }

    // set start button LED to red
    start_button_led.setPixelColor(0, RGB_COLOR_RED);
    start_button_led.show();

    // Move barrier down
    x1_motor.set_actuation_limit(100);
    x2_motor.set_actuation_limit(100);
    move_all_axes(x1_motor.get_speed_limit(), x1_motor.getSwLimitMinus());

    // Wait for 20 seconds before raising the barrier again
    delay(1000 * 20);
}