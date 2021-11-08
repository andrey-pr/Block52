#include "drivers.h"
#include "tasks/sensorsSurvey/sensorsSurvey.h"
#include "tasks/ledControl/ledControl.h"

UART_HandleTypeDef usartHandle;

int main()
{
    hardware_init();
    freertos_init();
    led_init(STAIRS * LEDS_ON_STAIR);
    xTaskCreate(sensorsSurvey, "sensorsSurvey", 200, NULL, 1, NULL);
    xTaskCreate(ledControl, "ledControl", 50, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;)
    {
    }
}