#include "drivers.h"

int main()
{
    hardware_init();
    led_init(2);
    led_setPixel(0, 64, 0, 64);
    led_setPixel(1, 64, 0, 64);
    while (true)
    {
        while (!led_isReady())
            ;
        led_setPixel(0, 128, 0, 64);
        led_setPixel(1, 64, 0, 128);
        led_pushFrameboofer();
        delay(100);
        while (!led_isReady())
            ;
        led_setPixel(0, 64, 0, 128);
        led_setPixel(1, 128, 0, 64);
        led_pushFrameboofer();
        delay(100);
    }
}