#include <stm32g070xx.h>

#include "time/time.h"

static void initRCC(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;

    FLASH->ACR &=~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_1;
    /*
    Setting clock to 64MHz
    - PLLM 1
    - PLLN 8
    - PLLR 2
    - PLLP 2
    - AHB 1
    - APB 1
    */
    RCC->CR |= RCC_CR_HSION; // turn HSI on
    while (!(RCC->CR & RCC_CR_HSIRDY))
    {
    };     
    RCC->CFGR = 0x0; // reset CFGR register - source clock is set as HSI by default
    RCC->PLLCFGR =0x0;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLPEN;
   

    RCC->CR |= RCC_CR_PLLON; // turn PLL on
    while (!(RCC_CR_PLLRDY & RCC->CR))
    { // wait for PLL
    };

    RCC->CFGR |= RCC_CFGR_SW_1;
    while (!(RCC_CFGR_SWS_1 & RCC->CFGR))
    {
    };

SystemCoreClockUpdate();
}

static void initGPIO(void)
{
//PA10 Led Pin
GPIOA->MODER &= ~GPIO_MODER_MODE10;
GPIOA->MODER |= GPIO_MODER_MODE10_0;


}

static void initSPI1(void)
{         



}

static void initTIM15(void)
{
   RCC->APBENR2 |= RCC_APBENR2_TIM15EN;

    TIM15->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    TIM15->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;

    TIM15->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;

    // channel 1 enable:
    TIM15->CCER |= TIM_CCER_CC1E;
    // channel 4 enable:
    TIM15->CCER |= TIM_CCER_CC4E;
    // Clock is 168 MHz
    TIM15->PSC = 72 - 1;  
    TIM15->ARR = 2500 - 1; // 1 period of PWM frequency is set to 400 Hz

    TIM15->CCR1 = 2500; // PWM length channel 1 (1 [ms])
    TIM15->CCR4 = 200; // PWM length channel 4 (1 [ms])
    TIM15->EGR |= TIM_EGR_UG;
}

static void initTIM1()
{
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM1->CCMR2 |= TIM_CCMR1_CC2S_0;
    TIM1->CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);
    TIM1->SMCR |= TIM_SMCR_SMS_0|TIM_SMCR_SMS_1;
    TIM1->CR1 |= TIM_CR1_CEN;
}

static void initTIM3()
{
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM3->CCMR2 |= TIM_CCMR1_CC2S_0;
    TIM3->CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);
    TIM3->SMCR |= TIM_SMCR_SMS_0|TIM_SMCR_SMS_1;
    TIM3->CR1 |= TIM_CR1_CEN;
}


static void initTIM6(void)
{
    // free running timer for time counting
    RCC->APBENR1 |= RCC_APBENR1_TIM6EN;
    TIM6->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    TIM6->PSC = SystemCoreClock/1E6 -1;      // 1us 1 count
    TIM6->ARR = 65536 - 1; // reload every second
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->EGR |= TIM_EGR_UG;
   

}


static void initEXTI(void)
{
    /*
EXTI->RTSR |= (EXTI_RTSR_TR4|EXTI_RTSR_TR5);    //Enable Rising edge interrupt for PA5&6
EXTI->IMR |= (EXTI_IMR_MR4|EXTI_IMR_MR5);
*/
}

static void initNVIC(void)
{
    /*
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 6);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 6);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 6);
    NVIC_SetPriority (DMA1_Channel3_IRQn, 0);
    NVIC_SetPriority (DMA1_Channel2_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(USART3_IRQn);
    */
   NVIC_EnableIRQ(TIM6_IRQn);
}

static uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

static void initUART3()
{
    /*
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; // AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13;       // apb2 prescaler bits 13:15
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; // fancy table stolen from std_periph
    baud_rate /= 115200;
    USART3->BRR = (uint16_t)baud_rate;
    USART3->CR1 = USART_CR1_RE | USART_CR1_TE; // enable uart rx and tx
    USART3->CR1 |= USART_CR1_IDLEIE;           // enable idle interrupt
    USART3->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // enable dma tx_rx
    USART3->CR1 |= USART_CR1_UE;
    */
}

static void initDMA()
{
   
}

void SetupSystem()
{
    initRCC();
    initNVIC();
    initGPIO();

    initTIM1();
    initTIM3();
    initTIM6();
    initTIM15();
  
    initEXTI();
    initDMA();

    initUART3();

    initSPI1();

    TicksInit();
    
}