#pragma once

#include <stdbool.h>
#include <stm32g070xx.h>


void GPIOTogglePin(GPIO_TypeDef* bus, uint16_t pin);
void GPIOSetPin(GPIO_TypeDef* bus, uint16_t pin);
void GPIOResetPin(GPIO_TypeDef* bus, uint16_t pin);
void GPIOSetPinState(GPIO_TypeDef* bus, uint16_t pin, bool state);