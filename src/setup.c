#include <stm32g070xx.h>

#include "config.h"
#include "time/time.h"

static void initRCC(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_FLASHEN;
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

//PA2 PA3 - motor PWM pins
//PA4 PA5 motor direction pin
GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_2 ;
GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_2;
GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3;

//PA6 PA7 PB0
GPIOA->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
GPIOA->MODER |= GPIO_MODER_MODE7_1 | GPIO_MODER_MODE6_1;
GPIOB->MODER &= ~GPIO_MODER_MODE0;
GPIOA->AFR[0] |= GPIO_AFRL_AFSEL6_0;
GPIOA->AFR[0] |= GPIO_AFRL_AFSEL7_0;

GPIOB->MODER &= ~GPIO_MODER_MODE1;

//PA9 PA8 Encoder 2
GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE8);
GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1;
//GPIOB->MODER &= ~GPIO_MODER_MODE0;
GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_1;
GPIOA->AFR[1] |= GPIO_AFRH_AFSEL8_1;

//USART1 PB7&6
GPIOB->MODER &=~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
GPIOB->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
//Transoptor pin PB1
GPIOB->MODER &= ~(GPIO_MODER_MODE1 );
GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD1;
GPIOB->PUPDR |=GPIO_PUPDR_PUPD1_0;
}

static void initSPI1(void)
{         



}

static void initTIM15(void)
{
    RCC->APBENR2 |= RCC_APBENR2_TIM15EN;

    TIM15->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    TIM15->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIM15->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;

    // channel 1 enable:
    TIM15->CCER |= TIM_CCER_CC1E;
    // channel 2 enable:
    TIM15->CCER |= TIM_CCER_CC2E;
    // Clock is 168 MHz
    TIM15->PSC = 1 - 1;  
    TIM15->ARR = M_ARR; // 1 period of PWM frequency is set to 400 Hz

    TIM15->CCR1 = 0; // PWM length channel 1 (1 [ms])
    TIM15->CCR2 = 0; // PWM length channel 4 (1 [ms])
    TIM15->BDTR |= TIM_BDTR_MOE | TIM_BDTR_BKP;
    TIM15->EGR |= TIM_EGR_UG;
    TIM15->CR1 |= TIM_CR1_CEN;
}

static void initTIM1()
{
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0|TIM_CCMR1_CC2S_0;
    //TIM3->CCMR1 |= TIM_CCMR1_CC2S_0;
    TIM1->CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);
    TIM1->CCER |= (TIM_CCER_CC1E|TIM_CCER_CC2E);
    TIM1->SMCR |= TIM_SMCR_SMS_0;
    TIM1->EGR |= TIM_EGR_UG|TIM_EGR_CC1G|TIM_EGR_CC2G;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
}

static void initTIM3()
{
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0|TIM_CCMR1_CC2S_0;
    //TIM3->CCMR1 |= TIM_CCMR1_CC2S_0;
    TIM3->CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);
    TIM3->CCER |= (TIM_CCER_CC1E|TIM_CCER_CC2E);
    TIM3->SMCR |= TIM_SMCR_SMS_0;
    TIM3->EGR |= TIM_EGR_UG|TIM_EGR_CC1G|TIM_EGR_CC2G;
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
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

    // EXTI->RTSR1 |=EXTI_RTSR1_RT0;
    // EXTI->IMR1 |= (EXTI_IMR1_IM0);
    // EXTI->EXTICR[0] |= EXTI_EXTICR1_EXTI0_0;

    EXTI->RTSR1 |=EXTI_RTSR1_RT1;
    EXTI->IMR1 |= (EXTI_IMR1_IM1);
    EXTI->EXTICR[0] |= EXTI_EXTICR1_EXTI1_0;
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
   NVIC_EnableIRQ(EXTI0_1_IRQn);
   NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
   NVIC_EnableIRQ(DMA1_Channel1_IRQn);
   NVIC_EnableIRQ(USART1_IRQn);
}



static void initUART1()
{
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;

    volatile uint32_t baud_rate = SystemCoreClock;
   baud_rate = baud_rate; // AHB prescaler bits 7:4
    baud_rate /= 115200;
   
    USART1->BRR =(uint16_t)baud_rate;

    USART1->CR1 = USART_CR1_RE | USART_CR1_TE; // enable uart rx and tx
    USART1->CR1 |= USART_CR1_IDLEIE;           // enable idle interrupt
    USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // enable dma tx_rx
    USART1->CR1 |= USART_CR1_UE;

    
}


static void initDMA()
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    //RX -> Channel 1
    DMA1_Channel1->CCR |= DMA_CCR_MINC;
    DMA1_Channel1->CCR &=~DMA_CCR_DIR;
    DMA1_Channel1->CPAR = (uint32_t)&USART1->RDR;
    DMA1_Channel1->CCR |= DMA_CCR_TCIE;
    DMAMUX1_Channel0->CCR |= (50)<<DMAMUX_CxCR_DMAREQ_ID_Pos;


    //TX ->channel 2
    DMA1_Channel2->CCR |= DMA_CCR_MINC;
    DMA1_Channel2->CCR |= DMA_CCR_DIR;
    DMA1_Channel2->CPAR = (uint32_t)&USART1->TDR;
    DMA1_Channel2->CCR |= DMA_CCR_TCIE;
    DMAMUX1_Channel1->CCR |= (51)<<DMAMUX_CxCR_DMAREQ_ID_Pos;

    
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

    initUART1();

    initSPI1();

    TicksInit();
    
}