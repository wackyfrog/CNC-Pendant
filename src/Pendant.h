//
// Created by Oleksandr Degtiar on 16.03.2021.
//

#ifndef CNC_PENDANT_PENDANT_H
#define CNC_PENDANT_PENDANT_H

#include <Arduino.h>
#include "Config.h"
#include "RotaryEncoder.h"

class Pendant {
public:

    typedef enum {
        OFF,
        X, Y, Z, U
    } Axis;

    typedef enum {
        UNDEFINED = 0,
        X1, X10, X100
    } XSwitch;

    typedef enum {
        AwaitingForPress,
        Pressed,
        Released,
        AwaitingForRelease
    } ButtonState;

    typedef bool (*ButtonPressCallback)();

    typedef bool (*ButtonReleaseCallback)(uint32_t time);

    ButtonPressCallback onButtonPress;
    ButtonReleaseCallback onButtonRelease;

    Pendant();

    int getXSwitch() const;

    int getFeedFactor() const;

    int getAxis() const;

    int readEncoderValue();

    void resetEncoder();

    void poll();

    bool isButtonPressed();

    uint32_t getPollTime();

    uint32_t getButtonPressingTime();

private:
    uint32_t pollTime = 0;

    RotaryEncoder encoder;

    const int axisSwitchPins[4] = {PIN_AXIS_X, PIN_AXIS_Y, PIN_AXIS_Z, PIN_AXIS_U};
    int axis = OFF;

    const int xSwitchPins[3] = {PIN_X1, PIN_X10, PIN_X100};
    // 0 - off (or not connected)
    // 100 - means: (encoder value in cents) x 100
    // 10 - means: (encoder value in cents) x 10
    // 1 - means: (encoder value in cents) x 1
    const int xFeedFactor[4] = {0, 100, 10, 1};
    int xSwitch = UNDEFINED;

    bool pressedEarlier = false;
    uint32_t pressChangedAt = 0;

    ButtonState buttonState = AwaitingForPress;

    uint32_t buttonPressedAt = 0;

    void pollButton();

    void pollAxisSwitch();

    void pollXSwitch();

};

#endif //CNC_PENDANT_PENDANT_H
