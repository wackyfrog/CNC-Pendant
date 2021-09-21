//
// Created by Oleksandr Degtiar on 05.03.2021.
//

#ifndef CNC_PENDANT_CONFIG_H
#define CNC_PENDANT_CONFIG_H

#include <Arduino.h>

#define VERSION "CNCPendant v1.0RC for Duet3D"

// Serial port configuration
#define BAUD_RATE 57600

#if defined(__AVR_ATmega32U4__)     // Arduino Leonardo or Pro Micro
# define UartSerial   Serial1
#elif defined(__AVR_ATmega328P__)   // Arduino Uno or Nano
# define UartSerial   Serial
#endif

// Rotary encoder
#define PIN_ENCODER_A  2
#define PIN_ENCODER_B  3
#define ENCODER_PULSES_PER_CLICK 4

// Axis switch
#define PIN_AXIS_X 4
#define PIN_AXIS_Y 5
#define PIN_AXIS_Z 6
#define PIN_AXIS_U 7

// X switch
#define PIN_X1   A1
#define PIN_X10  A2
#define PIN_X100 A3

/* Multifunctional button with led */
// Active state (LOW)
#define BUTTON_PRESSED LOW
#define PIN_BUTTON 9

// 15 sec
#define MOVEMENT_IDLE_TIME ( 10 * 1000 )

// 5 sec
#define ACKNOWLEDGE_IDLE_TIME ( 5 * 1000 )

// Time to hold button in movement mode to run set WCS command
// 2 sec
#define BUTTON_HOLD_TIME_TO_SET_WCS ( 2 * 1000 )

#define BUTTON_SHORT_PRESS_MAX_TIME 1000

#define PIN_BUTTON_LED 8

#define BUTTON_DEBOUNCE_TIME 30


/* Led */
#define PIN_ACTIVITY_LED 10

#define HUE_RED                 0
#define HUE_ORANGE              30
#define HUE_YELLOW              60
#define HUE_CHARTREUSE_GREEN    90
#define HUE_GREEN               120
#define HUE_SPRING_GREEN        150
#define HUE_CYAN                180
#define HUE_AZURE               210
#define HUE_BLUE                240
#define HUE_VIOLET              270
#define HUE_MAGENTA             300
#define HUE_ROSE                330

#define SATURATION_MAX          100

#define LED_MAX_VAL 50

const uint16_t axisColor[] = {
        HUE_RED,      // X
        HUE_GREEN,    // Y
        HUE_BLUE,     // Z
        HUE_VIOLET    // U
};

// The minimum interval (in milliseconds) between sending movement commands.
#define MOVEMENT_SEND_INTERVAL 20
// Table of commands we send, one entry for each axis
const char *const MoveCommands[] = {
        "G91 G0 F6000 X",     // X axis
        "G91 G0 F6000 Y",     // Y axis
        "G91 G0 F600  Z",      // Z axis
        "G91 G0 F6000 U"
};

// Table of commands we send for set WCS (Work Coordinate System), one entry for each axis
const char *const WCSSetCommands[] = {
        "M98 P\"pendant/set-wcs-x.g\" ",     // X axis
        "M98 P\"pendant/set-wcs-y.g\" ",     // Y axis
        "M98 P\"pendant/set-wcs-z.g\" ",     // Z axis
        "M98 P\"pendant/set-wcs-u.g\" ",     // U axis
};

const char *const WCSResetCommands[] = {
        "M98 P\"pendant/reset-wcs-x.g\" ",     // X axis
        "M98 P\"pendant/reset-wcs-y.g\" ",     // Y axis
        "M98 P\"pendant/reset-wcs-z.g\" ",     // Z axis
        "M98 P\"pendant/reset-wcs-u.g\" ",     // U axis
};

// The interval (in milliseconds) between sending repeating pause commands.
#define CONTROL_COMMAND_RESEND_INTERVAL 1000

const char *const CommandConfirm = "M98 P\"pendant/cmd-confirm.g\" ";
const char *const CommandCancel = "M98 P\"pendant/cmd-cancel.g\" ";

#endif //CNC_PENDANT_CONFIG_H
