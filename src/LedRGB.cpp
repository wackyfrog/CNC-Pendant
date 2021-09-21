//
// Created by Oleksandr Degtiar on 14.03.2021.
//

#include "LedRGB.h"
#include "ws2812/ws2812.h"
#include "FastHSV2RGB/fast_hsv2rgb.h"

LedRGB::LedRGB(const int pin) : Led(pin) {
    portMask = digitalPinToBitMask(pin);
    port = portOutputRegister(digitalPinToPort(pin));
}

void LedRGB::setColor(Color newColor) {
    memcpy(newColor.raw, color.raw, sizeof(color.raw));
}

void LedRGB::setColor(uint8_t r, uint8_t g, uint8_t b) {
    color.r = r;
    color.g = g;
    color.b = b;
}

void LedRGB::out() {
    const uint8_t *RGB_OFF[3] = {0};
    ws2812_send(isLighted() ? color.raw : (uint8_t *) &RGB_OFF, sizeof(color.raw), portMask, port);
}

void LedRGB::setColorHSV(uint16_t hue, uint8_t saturation, uint8_t value) {
    fast_hsv2rgb_8bit(HSV_HUE_DEGREE(hue), HSV_SAT_PERCENT(saturation), HSV_VAL_PERCENT(value),
                      &color.r, &color.g, &color.b);
}

void LedRGB::setColorWhite(uint8_t value) {
    memset(color.raw, value, 3);
}

void LedRGB::setColor(uint32_t rgb) {
    const uint32_t mask8bit = 0xff;
    color.r = (uint8_t)(rgb & mask8bit);
    color.g = (uint8_t)((rgb >> 8) & mask8bit);
    color.b = (uint8_t)((rgb >> 16) & mask8bit);
}
