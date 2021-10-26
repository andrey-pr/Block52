#pragma once

#include "defines.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
void freertos_init();
void delay(long ms);