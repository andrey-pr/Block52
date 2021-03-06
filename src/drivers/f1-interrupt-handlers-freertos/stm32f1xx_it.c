#include "stm32f1xx_it.h"

extern void xPortSysTickHandler(void);
void HardFault_Handler(void)
{
  while (1)
  {
  }
}
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        /* call the FreeRTOS kernel for a tick update*/
        xPortSysTickHandler();
    }
}