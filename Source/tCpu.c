//
// Created by ryzcq on 24-8-7.
//
#include "main.h"
#include "MyRTOS.h"


void tSetSysTickPeriod(uint32_t ms)
{
    SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1;
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
}
