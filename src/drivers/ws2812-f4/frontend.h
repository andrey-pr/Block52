#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ws2812b.h"

void led_init(int pcs);
void led_setPixel(int pos, uint8_t red, uint8_t green, uint8_t blue);
bool led_isReady();
void led_pushFrameboofer();