#include "drivers.h"


void myTask1(void *pvParameters);

int main()
{
    hardware_init();
    led_init(1);
    sensors_init();
    freertos_init();
    xTaskCreate(myTask1, "Task1", 50, NULL, 1, NULL);

    vTaskStartScheduler();
}

void myTask1(void *pvParameters)
{
    while (true)
    {
        while (!led_isReady())
            ;
        if (sensors_IsCrossed(LOWER_OUTER_SENSOR))
        {
            led_setPixel(0, 128, 0, 64);
        }
        led_pushFrameboofer();
        delay(100);
        while (!led_isReady())
            ;
        if (sensors_IsCrossed(LOWER_INNER_SENSOR))
        {
            led_setPixel(0, 64, 0, 128);
        }
        led_pushFrameboofer();
        delay(100);
    }
}