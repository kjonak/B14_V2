#include "time/time.h"
#include "PID.h"
#include "Variables.h"
#include "odometry.h"
static timeUs_t last_time;



static PID_t pid_Velocity, pid_angle;
static float gain_velocity, gain_angle;
static float acceleration_time, angular_acceleration_time;
static float M1_out, M2_out;

static float control_velocity(float dt);
static float control_angle(float dt);
static void update_motors(float M1, float M2);

void CTRL_task(timeUs_t currentTime)
{
    float dt_seconds = (float)(micros()-last_time)/1.0E6;
    M1_out = control_velocity(dt_seconds);
    M2_out = control_angle(dt_seconds);
    update_motors(M1_out, M2_out);
    last_time = micros();
}

void CTRL_Init()
{
    PID_initialize(&pid_Velocity,1,1,1,1);
    PID_initialize(&pid_angle,1,1,1,1);
    gain_velocity = 1;
    gain_angle = 1;
    acceleration_time = 0.1;
    angular_acceleration_time = 0.1;
}

static float control_velocity(float dt)
{
 float velocity_error = VAR_GetFloat(VAR_VELOCITY) - VAR_GetFloat(VAR_DESIRED_VELOCITY);
 float desired_acceleration =(velocity_error)/acceleration_time;
 VAR_SetFloat(desired_acceleration, VAR_DESIRED_ACCELERATION);
 float acceleration_error = VAR_GetFloat(VAR_ACCELERATION) - desired_acceleration;
 PID_update(&pid_Velocity, acceleration_error, dt);
 return gain_velocity * velocity_error + pid_Velocity.output;
}

static float control_angle(float dt)
{
 float ang_velocity_error = VAR_GetFloat(VAR_ANGULAR_VELOCITY) - VAR_GetFloat(VAR_DESIRED_ANGULAR_VELOCITY);
 float desired_ang_acceleration =(ang_velocity_error)/acceleration_time;
 VAR_SetFloat(desired_ang_acceleration, VAR_DESIRED_ANGULAR_ACCELERATION);
 float ang_acceleration_error = VAR_GetFloat(VAR_ANGULAR_ACCELERATION) - desired_ang_acceleration;
 PID_update(&pid_angle, ang_acceleration_error, dt);
 return gain_angle * ang_velocity_error + pid_angle.output;
}

static void update_motors(float M1, float M2)
{
    return;
}