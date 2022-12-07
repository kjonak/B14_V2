#include <stm32g070xx.h>
#include <stdbool.h>


void GPIOTogglePin(GPIO_TypeDef* bus, uint16_t pin){
    ((GPIO_TypeDef*)bus)->ODR ^=(1<<pin);
}

void GPIOSetPin(GPIO_TypeDef* bus, uint16_t pin){
     //   ((GPIO_TypeDef*)bus)->BSRR = (uint16_t)(1<<pin);
        bus->ODR |= 1<<pin;
}

void GPIOResetPin(GPIO_TypeDef* bus, uint16_t pin)
{
    bus->ODR &= ~(1<<pin);
//((GPIO_TypeDef*)bus)->BSRR = (uint16_t)((1<<16<<(pin)));
}

void GPIOSetPinState(GPIO_TypeDef* bus, uint16_t pin, bool state)
{
    if(state)
        bus->ODR |= 1<<pin;
    else
        bus->ODR &= ~(1<<pin);
    //bus->BSRR = (state==false)?(uint16_t)((1<<16<<(pin))):(uint16_t)((1<<(pin)));
//((GPIO_TypeDef*)bus)->BSRR = (state==false)?(uint16_t)((1<<16<<(pin))):(uint16_t)((1<<(pin)));
}