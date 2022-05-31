//
// Created by Oleksandr Degtiar on 16.03.2021.
//

#include "Pendant.h"
#include "Led.h"
#include "LedRGB.h"

int Pendant::getXSwitch() const {
    return xSwitch;
}

int Pendant::getFeedFactor() const {
    return xFeedFactor[xSwitch];
}

int Pendant::getAxis() const {
    return axis;
}

void Pendant::pollXSwitch() {
    xSwitch = UNDEFINED;
    int currentXSwitch = UNDEFINED;
    for (int pin : xSwitchPins) {
        ++currentXSwitch;
        if (digitalRead(pin) == LOW) {
            xSwitch = currentXSwitch;
            break;
        }
    }

//    xSwitchPrevious = xSwitch;
}

void Pendant::pollAxisSwitch() {
    axis = OFF;
    int currentAxis = OFF;
    for (int pin : axisSwitchPins) {
        ++currentAxis;
        if (digitalRead(pin) == LOW) {
            axis = currentAxis;
            break;
        }
    }
//    axisPrevious = axis;
}

/**
 *
 * @return true If state handled by button.
 */
void Pendant::pollButton() {
    bool pressed = digitalRead(PIN_BUTTON) == BUTTON_PRESSED;
    if (pressed != pressedEarlier) {
        pressChangedAt = pollTime;
        pressedEarlier = pressed;
        return;
    }

    if (pollTime - pressChangedAt < BUTTON_DEBOUNCE_TIME) {
        return;
    }

    switch (buttonState) {
        case AwaitingForPress:
            if (pressed) {
                buttonState = Pressed;
                buttonPressedAt = pollTime;
            } else {
                break;
            }
            // fallthrough if state changed AwaitingForPress -> Pressed

        case Pressed:
            if (!pressed) {
                buttonState = Released;
                break;
            }

            if (onButtonPress == nullptr || onButtonPress()) {
                buttonState = AwaitingForRelease;
            }
            break;

        case AwaitingForRelease:
            if (!pressed) {
                buttonState = Released;
            } else {
                break;
            }
            // fallthrough if state changed AwaitingForRelease -> Released

        case Released:
            if (onButtonRelease == nullptr || onButtonRelease(pollTime - buttonPressedAt)) {
                buttonState = AwaitingForPress;
            }
            break;
    }
}

int Pendant::readEncoderValue() {
    return encoder.getChange();
}

void Pendant::resetEncoder() {
    encoder.reset();
}

void Pendant::poll() {
    pollTime = millis();

    // Poll the encoder. Ideally we would do this in the tick ISR, but after all these years the Arduino core STILL doesn't let us hook it.
    // We could possibly use interrupts instead, but if the encoder suffers from contact bounce then that isn't a good idea.
    // In practice this loop executes fast enough that polling it here works well enough
    encoder.poll();

    pollAxisSwitch();
    pollXSwitch();
    pollButton();
}

Pendant::Pendant() :
        encoder(PIN_ENCODER_A, PIN_ENCODER_B, ENCODER_PULSES_PER_CLICK) {
    for (int pin : axisSwitchPins) {
        pinMode(pin, INPUT_PULLUP);
    }
    for (int pin : xSwitchPins) {
        pinMode(pin, INPUT_PULLUP);
    }

    pinMode(PIN_BUTTON, INPUT_PULLUP);
}

bool Pendant::isButtonPressed() {
    return buttonState != AwaitingForPress;
}

uint32_t Pendant::getPollTime() {
    return pollTime;
}

uint32_t Pendant::getButtonPressingTime() {
    return isButtonPressed() ? pollTime - buttonPressedAt : 0;
}

