#include <stm32g070xx.h>
#include "time.h"

static timeUs_t time;

void TIM6_IRQHandler()
{
        if (TIM_SR_UIF & TIM6->SR)
    {
        TIM6->SR &= ~TIM_SR_UIF;
        time += (TIM6->ARR + 1);
    }
}

void TicksInit(void)
{
    time = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
}

timeUs_t micros(void)
{
    return time + TIM6->CNT;
}

void DelayUs(uint64_t us)
{
    timeUs_t t = micros();
    while (micros() - t < us)
    {
        ; // wait
    }
}

void DelayMs(uint64_t ms)
{
    timeUs_t t = micros();
    while (micros() - t < ms * 1000)
    {
        ; // wait
    }
}