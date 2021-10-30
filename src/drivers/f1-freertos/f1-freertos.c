#include "f1-freertos.h"

void freertos_init()
{
    qHandle = xQueueCreate(3, sizeof(struct UserOnStairs));
}

void delay(long ms)
{
    vTaskDelay(ms * 1000 / configTICK_RATE_HZ);
}