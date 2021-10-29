#pragma once
#include "stm32f4xx_hal.h"

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