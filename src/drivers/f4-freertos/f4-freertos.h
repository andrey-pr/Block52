#pragma once

#include "defines.h"
void freertos_init();
void vPortSetupTimerInterrupt(void);
void RTOS_TICK_TIMER_IRQHandler(void);