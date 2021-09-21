//
// Created by Oleksandr Degtiar on 23.01.2021.
//

#ifndef WS2812_H
#define WS2812_H

#include <avr/io.h>

void ws2812_send(uint8_t *data, uint8_t datlen, uint8_t maskHi, volatile uint8_t *port);

#endif //WS2812_H
