//
// Created by Oleksandr Degtiar on 14.03.2021.
//

#ifndef CNC_PENDANT_LEDRGB_H
#define CNC_PENDANT_LEDRGB_H

#include <Arduino.h>
#include "Led.h"
#include "FastHSV2RGB/fast_hsv2rgb.h"

#define HSV_HUE_DEGREE(x) ( x > 359 ? HSV_HUE_MAX : (((uint32_t)x * (uint32_t)HSV_HUE_MAX) / 360) )
#define HSV_VAL_PERCENT(x) ( (uint8_t) ( (uint16_t)x * (uint16_t)HSV_VAL_MAX / 100) )
#define HSV_SAT_PERCENT(x) ( (uint8_t) ( (uint16_t)x * (uint16_t)HSV_SAT_MAX / 100 ) )

class LedRGB : public Led {

public:
    typedef struct {
        uint8_t raw[3] = {0};
        uint8_t &r = raw[1];
        uint8_t &g = raw[0];
        uint8_t &b = raw[2];
    } Color;


    LedRGB(const int pin);

    void setColor(uint8_t r, uint8_t g, uint8_t b);

    void setColor(Color newColor);

    void setColorHSV(uint16_t hue, uint8_t saturation, uint8_t value);

    void setColor(uint32_t rgb);

    void setColorWhite(uint8_t value);

private:
    uint8_t portMask;
    volatile uint8_t *port;
    Color color;

    void out();
};


#endif //CNC_PENDANT_LEDRGB_H
