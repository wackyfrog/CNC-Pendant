#ifndef __RotaryEncoderIncluded
#define __RotaryEncoderIncluded

#include <Arduino.h>

class RotaryEncoder {
    uint8_t state;
    int pin0, pin1;
    int ppc;
    int change;

    uint8_t readState() {
        return (digitalRead(pin0) ? 1u : 0u) | (digitalRead(pin1) ? 2u : 0u);
    }

public:
    RotaryEncoder(int p0, int p1, int pulsesPerClick);

    void poll();

    int getChange();

};

#endif
