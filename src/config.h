#pragma once
#include <stm32g070xx.h>

#define ENCODER_PPR 1280.0
#define SHAFT_PPR 10


/****************PIN CONFIGURATION****************/
#define LED_HB_PIN 10
#define LED_HB_BUS GPIOA


#define LED_PC_CONNECTED_PIN 0
#define LED_PC_CONNECTED_BUS GPIOB

#define LED_LOW_BAT_PIN 1
#define LED_LOW_BAT_BUS GPIOB

#define LED_PAD_CONNECTED_PIN 2
#define LED_PAD_CONNECTED_BUS GPIOB

#define FLASH_WP_PIN 12
#define FLASH_WP_BUS GPIOC

#define FLASH_HLD_PIN 2
#define FLASH_HLD_BUS GPIOD

#define FLASH_CS_PIN 11
#define FLASH_CS_BUS GPIOC

#define M1_A_PIN 12
#define M1_A_BUS GPIOA
#define M1_B_PIN 10
#define M1_B_BUS GPIOA

#define M2_A_PIN 9
#define M2_A_BUS GPIOA
#define M2_B_PIN 7
#define M2_B_BUS GPIOA