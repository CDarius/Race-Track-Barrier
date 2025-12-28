#pragma once

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

// ****************************************************

#define START_BUTTON_PRESSED    (digitalRead(START_BUTTON_PIN) == LOW)