#pragma once

#include "ws2812b.h"
#include "stdbool.h"
#include "stdlib.h"

void led_init(int pcs);
void led_setPixel(int pos, uint8_t red, uint8_t green, uint8_t blue);
void led_cleanFrameboofer();
bool led_isReady();
void led_pushFrameboofer();