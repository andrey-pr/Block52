#include "f4-freertos.h"

TIM_HandleTypeDef TIM_InitStruct;
extern void xPortSysTickHandler( void );

void freertos_init()
{
    NVIC_SetPriorityGrouping(0U);
}

void vPortSetupTimerInterrupt(void)
{
    RTOS_TICK_TIMER_CLK_ENABLE();

    /*
        From STM32F407 datasheet, Timer is clocked from
        APB1 bus (42Mhz max). In default configuration
        Timer is receiving 16Mhz (HSI) bus clock.
    */

    /***************************************************
                   Timer Configuration:
    ****************************************************/

    /* Select Timer for further configuration */
    TIM_InitStruct.Instance = RTOS_TICK_TIMER;

    /*
        Divide the timer input frequency (16Mhz)
        by a factor of 1000 (16,000,000/1,000 = 16,000 = 16Khz) 
    */
    TIM_InitStruct.Init.Prescaler = 1000;

#if (UP_COUNTER)
    /* Up-Counter mode*/
    TIM_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
#else
    TIM_InitStruct.Init.CounterMode = TIM_COUNTERMODE_DOWN;
#endif

    /*
        As configTICK_RATE_HZ = 1000, so tick timer
        need to generate interrupt at the rate of 1000/sec (1msec delay).
        As the input frequency is 16khz so the total
        counts required for 1msec delay:
        
        total counts = 1msec * f
                     =  0.001 * 16,000
                     = 16
    */
    TIM_InitStruct.Init.Period = 16;

    /*
        Finally initialize Timer
    */
    while (HAL_TIM_Base_Init(&TIM_InitStruct) != HAL_OK)
        ;

    /* Enable interrupt at IRQ-Level */
    HAL_NVIC_EnableIRQ(RTOS_TICK_TIMER_IRQn);

    /* 
        Tick timer should have least priority
        In STM32F4, the lowest Prioirty is 0xf.
    */
    NVIC_SetPriority(RTOS_TICK_TIMER_IRQn, 0x0fU);

    /*
        Start Timer and enable timer IRQ interrupt
    */
    HAL_TIM_Base_Start_IT(&TIM_InitStruct);
}

void RTOS_TICK_TIMER_IRQHandler(void)
{
    __HAL_TIM_CLEAR_IT(&TIM_InitStruct, TIM_IT_UPDATE);

    /*
        wait until the scheduler starts.
        This step is particularly important. if 
        timer (tick timer here) generate interrupt
        before the scheduler is ready, a Hard Fault
        exception will occur.
    */
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        xPortSysTickHandler();
    }
}