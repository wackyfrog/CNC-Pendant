//
// Created by Oleksandr Degtiar on 03.03.2021.
//

#include "Arduino.h"
#include "Led.h"

void Led::update() {
    if (updater != nullptr) {
        updater(this);
    }
    out();
    lastUpdateTime = millis();
}

void Led::out() {
    digitalWrite(pin, lighten ? HIGH : LOW);
}

void Led::on(uint32_t timeout) {
    setMode(On);
    offAtTime = millis() + timeout;
}

void Led::on() {
    setMode(On);
}

void Led::off() {
    setMode(Off);
}

void Led::tick() {
    if (mode == Off) {
        return;
    }

    if (offAtTime != 0 && millis() > offAtTime) {
        setMode(Off);
        return;
    }

    if (nextUpdateTime == 0 || millis() > nextUpdateTime) {
        update();
    }

}

void Led::setMode(Led::Mode newMode, Callback newUpdater) {
//    if (mode == newMode && updater == newUpdater) {
//        return;
//    }
    offAtTime = 0;
    switch (newMode) {
        case Off:
            lighten = false;
            updater = nullptr;
            break;

        case On:
            lighten = true;
            updater = nullptr;
            break;

        case Blink1:
            lighten = Off;
            updater = [](void *led) {
                Led *l = (Led *) led;
                l->nextUpdateTime = millis() + (l->lighten ? 250 : 250);
                l->toggleLight();
            };
            break;

        case FastBlink:
            lighten = Off;
            updater = [](void *led) {
                Led *l = (Led *) led;
                l->nextUpdateTime = millis() + (l->lighten ? 50 : 50);
                l->toggleLight();
            };
            break;

        case Blink2:
            lighten = Off;
            updater = [](void *led) {
                Led *l = (Led *) led;
                uint32_t now = millis();

                l->lighten = (l->state % 2 == 0);
                if (l->state < 5) {
                    l->nextUpdateTime = now + 100;
                    l->state++;
                } else {
                    l->nextUpdateTime = now + 500;
                    l->state = 0;
                }
            };
            break;

        case Custom:
            lighten = Off;
            updater = newUpdater;
            if (newUpdater == nullptr) {
                mode = Off;
            }
            break;
    }

    nextUpdateTime = 0;
    state = 0;
    mode = newMode;
    update();
}

bool Led::isLighted() const {
    return lighten;
}

Led::Mode Led::getMode() const {
    return mode;
}

uint32_t Led::getNextUpdateTime() const {
    return nextUpdateTime;
}

uint8_t Led::getState() const {
    return state;
}

Led::Led(const int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
    off();
}

void Led::setLighten(bool light) {
    lighten = light;
}

void Led::setNextUpdateTime(uint32_t time) {
    nextUpdateTime = time;
}

void Led::setState(uint8_t newState) {
    state = newState;
}

void Led::toggleLight() {
    lighten = !lighten;
}

bool Led::isOff() {
    return mode == Off;
}

bool Led::isDynamic() {
    return offAtTime != 0 || nextUpdateTime != 0;
}

uint32_t Led::getLastUpdateTime() const {
    return lastUpdateTime;
}

