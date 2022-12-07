#include <stdbool.h>
#include <stm32g070xx.h>
#include "config.h"
#include "drivers/GPIO.h"
static bool motors_enabled =false;

static float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void MOTORS_SetOutputs(float fwd, float rot)
{
    if(!motors_enabled)
        return;
    rot = 0.1;
    float M1,M2;
    bool M1_dir = true;
    bool M2_dir = true;

    M1 = map(fwd,-1.f,1.f,-M_ARR,M_ARR);
    M2 = map(rot,-1.f,1.f,-M_ARR,M_ARR);

    if(M1<0)
    {
        M1_dir = false;
        M1*=-1;
    }
     if(M2<0)
    {
        M2_dir = false;
        M2*=-1;
    }

    GPIOSetPinState(M1_DIR_BUS,M1_DIR_PIN, M1_dir);
    GPIOSetPinState(M2_DIR_BUS,M2_DIR_PIN, M2_dir);

    TIM15->CCR1 = (uint16_t)M1;
    TIM15->CCR2 = (uint16_t)M2;


}

void MOTORS_Enable()
{
    motors_enabled =true;
}
void MOTORS_Disable()
{
    MOTORS_SetOutputs(0,0);
    motors_enabled = false;
}
