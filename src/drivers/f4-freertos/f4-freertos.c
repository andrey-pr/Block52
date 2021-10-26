#include "f4-freertos.h"

void freertos_init()
{
    NVIC_SetPriorityGrouping(0U);
}

void delay(long ms)
{
    vTaskDelay(ms * 1000 / configTICK_RATE_HZ);
}