#pragma once

#ifdef DISCO_TESTBOARD
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
  void SysTick_Handler(void);
#ifdef __cplusplus
}
#endif