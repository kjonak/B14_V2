#include <stm32g070xx.h>
#include <stdint.h>
#include <string.h>
#include "Variables.h"
#include "odometry.h"
#include "config.h"
#include "time/time.h"

typedef struct 
{
    float *distance;
    float *velocity;
    float *acceleration;
    float *rotation;
    float *ang_velocity;
    float *ang_acceleration;
}ODO_data_t;

static int32_t counter_1 = 0;
static int32_t counter_2 = 0;

static int32_t *counter_angle, *counter_distance;
static ODO_data_t data;
static timeUs_t last_time;

void EXTI9_5_IRQHandler()
{
   //if(EXTI_PR_PR5 & EXTI->PR)
    {
        counter_1 += (int16_t)TIM3->CNT;
        TIM3->EGR |= TIM_EGR_UG;
      //  EXTI->PR |= EXTI_PR_PR5;
    }
}

void EXTI4_IRQHandler()
{
   // counter_2 += (int16_t)TIM2->CNT;
   // TIM2->EGR |= TIM_EGR_UG;
    //EXTI->PR |= EXTI_PR_PR4;
}


static void update_distance(float dt)
{
    float d_distance = (*counter_distance)/ENCODER_PPR;
    float new_velocity = d_distance/dt;
    *data.acceleration = (*data.velocity - new_velocity)/dt;
    *data.velocity = new_velocity;
    *data.distance+= d_distance;
    *counter_distance = 0;
}

static void update_rotation(float dt)
{
    float d_rotation = (*counter_angle)/ENCODER_PPR;
    float new_ang_velocity = d_rotation/dt;
    *data.ang_acceleration = (*data.velocity - new_ang_velocity)/dt;
    *data.ang_velocity = new_ang_velocity;
    *data.rotation += d_rotation;
    *counter_angle = 0;
}

void ODO_Init(void)
{
    counter_angle = &counter_2;
    counter_distance = &counter_1;
    memset(&data, 0, sizeof(ODO_data_t));


    data.distance = VAR_GetFloatPtr(VAR_DISTANCE);
    data.velocity = VAR_GetFloatPtr(VAR_VELOCITY);
    data.acceleration = VAR_GetFloatPtr(VAR_ACCELERATION);
    data.rotation = VAR_GetFloatPtr(VAR_HEADING);
    data.ang_velocity = VAR_GetFloatPtr(VAR_ANGULAR_VELOCITY);
    data.ang_acceleration = VAR_GetFloatPtr(VAR_ANGULAR_ACCELERATION);

    last_time = micros();
}

void ODO_task(timeUs_t currentTime)
{
    float dt_seconds = (float)(micros()-last_time)/1.0E6;
    if(dt_seconds<=0)
        return;
    update_distance(dt_seconds);
    update_rotation(dt_seconds);
    last_time = micros();
    return;
}