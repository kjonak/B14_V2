#include "config.h"
#include "drivers/GPIO.h"

void LED_HB_Set()
{
    GPIOSetPin(LED_HB_BUS, LED_HB_PIN);
}
void LED_HB_Reset()
{
    GPIOResetPin(LED_HB_BUS, LED_HB_PIN);
}

void LED_HB_Toggle()
{
    GPIOTogglePin(LED_HB_BUS, LED_HB_PIN);
}