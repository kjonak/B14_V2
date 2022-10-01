#include <stm32g070xx.h>
#include <stdbool.h>

void GPIOTogglePin(GPIO_TypeDef* bus, uint16_t pin){
    ((GPIO_TypeDef*)bus)->ODR ^=(1<<pin);
}

void GPIOSetPin(GPIO_TypeDef* bus, uint16_t pin){
        ((GPIO_TypeDef*)bus)->BSRR = (uint16_t)(1<<pin);
}

void GPIOResetPin(GPIO_TypeDef* bus, uint16_t pin)
{
((GPIO_TypeDef*)bus)->BRR = (uint16_t)((1<<(pin)));
}