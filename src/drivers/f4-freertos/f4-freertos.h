#pragma once

#include "defines.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
void freertos_init();
void vPortSetupTimerInterrupt(void);
void RTOS_TICK_TIMER_IRQHandler(void);