#include "drivers.h"
#include "tasks/sensorsSurvey/sensorsSurvey.h"
#include "tasks/ledControl/ledControl.h"

UART_HandleTypeDef usartHandle;

int main()
{
    hardware_init();
    led_init(STAIRS * LEDS_ON_STAIR);
    sensors_init();
    freertos_init();
    xTaskCreate(sensorsSurvey, "sensorsSurvey", 50, NULL, 1, NULL);
    xTaskCreate(ledControl, "ledControl", 50, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;)
    {
    }
}