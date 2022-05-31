#ifndef __RotaryEncoderIncluded
#define __RotaryEncoderIncluded

#include <Arduino.h>

class RotaryEncoder {

public:
    RotaryEncoder(int p0, int p1, int pulsesPerClick);

    void poll();

    int getChange();

    void reset();

private:
    uint8_t state;
    int pin0, pin1;
    int ppc;
    int change;

    uint8_t readState();

};

#endif
