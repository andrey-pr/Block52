#pragma once
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C"
{
#endif
	void HardFault_Handler(void);
	void MemManage_Handler(void);
	void BusFault_Handler(void);
	void UsageFault_Handler(void);
	void DebugMon_Handler(void);
	void SysTick_Handler(void);
#ifdef __cplusplus
}
#endif