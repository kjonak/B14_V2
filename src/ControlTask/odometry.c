#include <stm32g070xx.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "Variables.h"
#include "odometry.h"
#include "config.h"
#include "time/time.h"
#include "IO/Motors.h"

typedef struct 
{
    float *distance;
    float *velocity;

    float *orientation;
    float *ang_velocity;

}ODO_data_t;

static int32_t counter_1 = 0;
static int32_t counter_2 = 0;

static int32_t *counter_angle, *counter_distance;
static ODO_data_t data;
static timeUs_t last_time;
static bool dist_readed = false;
static int16_t GetSpeedCNT(void)
{
    int16_t timer = (int16_t)TIM3->CNT;
    TIM3->EGR |= TIM_EGR_UG;
    return (timer);
}

static int16_t GetAngCNT(void)
{
    int16_t timer = (int16_t)TIM1->CNT;
    TIM1->EGR |= TIM_EGR_UG;
    return (timer);
}



static float prev_vel = 0;
static void update_distance(float dt)
{

    

    float d_distance = (GetSpeedCNT())/VELOCITY_ENCODER_PPR;

    float vel = d_distance/dt;
    float new_velocity = 0.854**data.velocity + 0.0728*vel + 0.0728*prev_vel;
    prev_vel = vel;

    *data.velocity = new_velocity;
    *data.distance+= d_distance;
     dist_readed = true;
     counter_1 = 0;
    
}
static int64_t rot_cnt = 0;
//transoptor interrupt
void EXTI0_1_IRQHandler()
{
     if(EXTI_RPR1_RPIF1 & EXTI->RPR1)
    {
        EXTI->RPR1 |= EXTI_RPR1_RPIF1;

        rot_cnt = 0;
    }
}

//not needed actualy :)
void EXTI4_IRQHandler()
{
   // counter_2 += (int16_t)TIM2->CNT;
   // TIM2->EGR |= TIM_EGR_UG;
    //EXTI->PR |= EXTI_PR_PR4;
}

static void update_rotation(float dt)
{
    int16_t cnt = -GetAngCNT();
    rot_cnt += cnt;
    //rot_cnt %= ROT_ENCODER_PPR;
    //now rot_cnt == pulses
    *data.orientation = rot_cnt/(float)ROT_ENCODER_PPR;

    float d_rotation = 6.28318530718f*((float)cnt)/(float)ROT_ENCODER_PPR;
    float new_ang_velocity = d_rotation/dt;
    
    *data.ang_velocity = new_ang_velocity;
    *data.orientation = 360.f*(float)rot_cnt/(float)ROT_ENCODER_PPR;
    if(*data.orientation<0)
        *data.orientation+=360.f;
    *counter_angle = 0;
}

void ODO_Init(void)
{
    counter_angle = &counter_2;
    counter_distance = &counter_1;
    memset(&data, 0, sizeof(ODO_data_t));


    data.distance = VAR_GetFloatPtr(VAR_DISTANCE);
    data.velocity = VAR_GetFloatPtr(VAR_VELOCITY);

    data.orientation = VAR_GetFloatPtr(VAR_HEADING);
    data.ang_velocity = VAR_GetFloatPtr(VAR_ANGULAR_VELOCITY);

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
void ODO_ResetDist()
{
    data.distance = 0;
}