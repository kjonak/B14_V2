#include "time/time.h"
#include "PID.h"
#include "Variables.h"
#include <string.h>
#include "odometry.h"
#include "config.h"
#include "IO/Motors.h"

static timeUs_t last_time;
static timeUs_t last_hb;

static float in_mode;
static PID_t pid_Velocity, pid_angle;
static float gain_velocity, gain_angle;
static float acceleration_time, angular_acceleration_time;
static float M1_out, M2_out;

static float control_velocity(float dt);
static float control_angle(float dt);
static void update_motors(float M1, float M2);

void CTRL_task(timeUs_t currentTime)
{
 //   if((micros()-last_hb)>CTRL_HB_TIMEOUT_US)
 //   {
 //       MOTORS_Disable();
 //       return;
//    }
     float dt_seconds = (float)(micros()-last_time)/1.0E6;
    if(in_mode == 1)
    {
        M1_out = VAR_GetFloat(VAR_DESIRED_VELOCITY);
        M2_out = VAR_GetFloat(VAR_DESIRED_ANGULAR_VELOCITY);
    }
    else if(in_mode ==0)
    {
        M1_out = control_velocity(dt_seconds);
        M2_out = control_angle(dt_seconds);
    }
   

    update_motors(M1_out, M2_out);
    last_time = micros();

}

void CTRL_NewHB()
{
    last_hb = micros();
}
void CTRL_StopMotors()
{
    MOTORS_Disable();
}
void CTRL_EnableMotors()
{
    MOTORS_Enable();
}
void CTRL_NewInput(uint8_t* data, uint8_t len)
{
float fwd, rot;
    memcpy(&in_mode, data, sizeof(float));
    memcpy(&fwd,data + sizeof(float),  sizeof(float));
    memcpy(&rot,data + 2*sizeof(float),  sizeof(float));

    VAR_SetFloat(fwd, VAR_DESIRED_VELOCITY);
    VAR_SetFloat(rot, VAR_DESIRED_ANGULAR_VELOCITY);
}

void CTRL_Init()
{
    PID_initialize(&pid_Velocity,1.9767f,42.4139,0.0034216,0.1);
    PID_initialize(&pid_angle,1,1,1,1);
    gain_velocity = 0.0;
    gain_angle = 1;
    acceleration_time = 0.1;
    angular_acceleration_time = 0.1;
}

static float control_velocity(float dt)
{
float d_velocity = + VAR_GetFloat(VAR_DESIRED_VELOCITY);
if((-MIN_VELOCITY<d_velocity) & (d_velocity<MIN_VELOCITY))
        d_velocity = 0;


 float velocity_error = -VAR_GetFloat(VAR_VELOCITY) + d_velocity;//VAR_GetFloat(VAR_DESIRED_VELOCITY);
 if((-MIN_VELOCITY_ERROR<velocity_error) & (velocity_error<MIN_VELOCITY_ERROR))
        velocity_error = 0;

 PID_update(&pid_Velocity, velocity_error, dt);
 return  pid_Velocity.output;
}

static float control_angle(float dt)
{
 float ang_velocity_error =VAR_GetFloat(VAR_DESIRED_ANGULAR_VELOCITY) - VAR_GetFloat(VAR_ANGULAR_VELOCITY);

 PID_update(&pid_angle, ang_velocity_error, dt);
 return  pid_angle.output;
}

static void update_motors(float M1, float M2)
{
    if(M1>1.f) M1 =1.f;
    if(M1<-1.f) M1=-1.f;
    if(M2>1.f)M2=1.f;
    if(M2<-1.f)M2=-1.f;
    if((-MIN_SIG<M1) & (M1<MIN_SIG))
        M1 = 0;
     if((-MIN_SIG<M2) & (M2<MIN_SIG))
        M2 = 0;
    MOTORS_SetOutputs(M1,M2);
    return;
}
static uint8_t pid_buffer[40];

uint8_t* CTRL_SerializePID(uint8_t* len)
{
    uint8_t* buffer = pid_buffer;
    
    memcpy(buffer, &pid_Velocity.P,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_Velocity.I,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_Velocity.D,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_Velocity.windup,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &gain_velocity,sizeof(float));
    buffer+=sizeof(float);

    memcpy(buffer, &pid_angle.P,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_angle.I,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_angle.D,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &pid_angle.windup,sizeof(float));
    buffer+=sizeof(float);
    memcpy(buffer, &gain_angle,sizeof(float));
    buffer+=sizeof(float);
    *len = 40;
    return pid_buffer;
    
}
void CTRL_UpdatePID(uint8_t* buffer, uint8_t len)
{
        if(len<10*sizeof(float))
        return;

        memcpy(&pid_Velocity.P, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_Velocity.I, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_Velocity.D, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_Velocity.windup, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&gain_velocity, buffer, sizeof(float));
        buffer+=sizeof(float);

        memcpy(&pid_angle.P, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_angle.I, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_angle.D, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&pid_angle.windup, buffer, sizeof(float));
        buffer+=sizeof(float);
        memcpy(&gain_angle, buffer, sizeof(float));
        buffer+=sizeof(float);

}