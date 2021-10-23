#include "delay.h"

void delay(int ms)
{
#ifdef INC_FREERTOS_H
#include "FreeRTOSConfig.h"
    vTaskDelay(ms * 1000 / configTICK_RATE_HZ);
#else
    HAL_Delay(ms);
#endif
}