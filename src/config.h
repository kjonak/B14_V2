#pragma once
#include <stm32g070xx.h>

#define VELOCITY_ENCODER_PPR 109512.f *0.98f/0.96f/1.01f
#define ROT_ENCODER_PPR 603840

#define MIN_SIG 0.1
#define MIN_VELOCITY 0.01
#define MIN_VELOCITY_ERROR 0.001
#define MIN_ANG_VELOCITY_ERROR 0.02
#define MAX_ACCELERATION 5.f
#define MAX_ANG_VELOCITY 0.5
/****************PIN CONFIGURATION****************/
#define LED_HB_PIN 10
#define LED_HB_BUS GPIOA

#define M_ARR 2500-1
#define M1_DIR_PIN 4
#define M1_DIR_BUS GPIOA

#define M2_DIR_PIN 5
#define M2_DIR_BUS GPIOA

#define CTRL_HB_TIMEOUT_US 500000