#include "time/time.h"
#include "PID.h"
#include "Variables.h"
#include <string.h>
#include <stdbool.h>
#include "odometry.h"
#include "config.h"
#include "IO/Motors.h"

static timeUs_t last_time,last_input;
static timeUs_t last_hb;

static float in_mode;
static PID_t pid_Velocity, pid_angle;
static float gain_angle, gain_velocity;
static float acceleration_time, angular_acceleration_time;
static float M1_out, M2_out;

static float control_velocity(float dt);
static float control_angle(float dt);
static float control_direction(float dt);
static void update_motors(float M1, float M2);

bool orientation_calibrated = false;

void CTRL_task(timeUs_t currentTime)
{
    //   if((micros()-last_hb)>CTRL_HB_TIMEOUT_US)
    //   {
    //       MOTORS_Disable();
    //       return;
    //    }
    float dt_seconds = (float)(micros() - last_time) / 1.0E6;
    if((micros()-last_input)>200000)
    {
        update_motors(0, 0);
        last_time = micros();
        return;
    }
    if (in_mode == 1)
    {
        M1_out = VAR_GetFloat(VAR_DESIRED_VELOCITY);
        M2_out = VAR_GetFloat(VAR_DESIRED_ANGULAR_VELOCITY);
    }
    else if (in_mode == 0)
    {
        M1_out = control_velocity(dt_seconds);
        M2_out = control_angle(dt_seconds);
    }
    else if (in_mode == 2)
    {
        M1_out = control_velocity(dt_seconds);
        M2_out = control_direction(dt_seconds);
    }
    if(!orientation_calibrated)
        {
            M1_out = 0; M2_out = 0.5;
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

void CTRL_NewInput(uint8_t *data, uint8_t len)
{
    last_input = micros();
    float fwd, rot;
    memcpy(&in_mode, data, sizeof(float));
    memcpy(&fwd, data + sizeof(float), sizeof(float));
    memcpy(&rot, data + 2 * sizeof(float), sizeof(float));

    if (in_mode == 2)
    {
        if ((rot < 0) || (rot > 360.f))
        {
            rot = VAR_GetFloat(VAR_HEADING);
            VAR_SetFloat(rot, VAR_DESIRED_HEADING);
        }
    }
    else
        VAR_SetFloat(rot, VAR_DESIRED_ANGULAR_VELOCITY);
    VAR_SetFloat(fwd, VAR_DESIRED_VELOCITY);
}

void CTRL_Init()
{
    PID_initialize(&pid_Velocity, 1.98f, 42.42, 0.0034, 0.1);
    PID_initialize(&pid_angle, 0, 14.42, 0, 1);
    gain_angle = 0.1;
    acceleration_time = 0.1;
    angular_acceleration_time = 0.1;
}

static float control_velocity(float dt)
{
    float d_velocity = VAR_GetFloat(VAR_DESIRED_VELOCITY);





    // if((-MIN_VELOCITY<d_velocity) & (d_velocity<MIN_VELOCITY))
    //         d_velocity = 0;

    float velocity_error = -VAR_GetFloat(VAR_VELOCITY) + d_velocity; // VAR_GetFloat(VAR_DESIRED_VELOCITY);
    //  if((-MIN_VELOCITY_ERROR<velocity_error) & (velocity_error<MIN_VELOCITY_ERROR))
    //         velocity_error = 0;

    PID_update(&pid_Velocity, velocity_error, dt);
    return pid_Velocity.output;
}

static float control_angle(float dt)
{
    float ang_velocity_error = VAR_GetFloat(VAR_DESIRED_ANGULAR_VELOCITY) - VAR_GetFloat(VAR_ANGULAR_VELOCITY);

    PID_update(&pid_angle, ang_velocity_error, dt);
    return pid_angle.output;
}

static float control_direction(float dt)
{
    float d_ang_vel;
    float heading_error = VAR_GetFloat(VAR_DESIRED_HEADING) - VAR_GetFloat(VAR_HEADING);
    if (heading_error > 180.f)
        heading_error -= 360.f;

    else if (heading_error < -180.f)
        heading_error += 360.f;

    d_ang_vel = heading_error * gain_angle;

    VAR_SetFloat(d_ang_vel, VAR_DESIRED_ANGULAR_VELOCITY);
    float ang_velocity_error = d_ang_vel - VAR_GetFloat(VAR_ANGULAR_VELOCITY);
    if ((-MIN_ANG_VELOCITY_ERROR < ang_velocity_error) & (ang_velocity_error < MIN_ANG_VELOCITY_ERROR))
        ang_velocity_error = 0;
    PID_update(&pid_angle, ang_velocity_error, dt);
    return pid_angle.output;
}
static void update_motors(float M1, float M2)
{
    if (M1 > 1.f)
        M1 = 1.f;
    if (M1 < -1.f)
        M1 = -1.f;
    if (M2 > 1.f)
        M2 = 1.f;
    if (M2 < -1.f)
        M2 = -1.f;
    if ((-MIN_SIG < M1) & (M1 < MIN_SIG))
        M1 = 0;
    if ((-MIN_SIG < M2) & (M2 < MIN_SIG))
        M2 = 0;
    MOTORS_SetOutputs(M1, M2);
    return;
}
static uint8_t pid_buffer[40];

uint8_t *CTRL_SerializePID(uint8_t *len)
{
    uint8_t *buffer = pid_buffer;

    memcpy(buffer, &pid_Velocity.P, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_Velocity.I, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_Velocity.D, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_Velocity.windup, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &gain_velocity, sizeof(float));
    buffer += sizeof(float);

    memcpy(buffer, &pid_angle.P, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_angle.I, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_angle.D, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &pid_angle.windup, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer, &gain_angle, sizeof(float));
    buffer += sizeof(float);
    *len = 40;
    return pid_buffer;
}
void CTRL_UpdatePID(uint8_t *buffer, uint8_t len)
{
    if (len < 10 * sizeof(float))
        return;

    memcpy(&pid_Velocity.P, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_Velocity.I, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_Velocity.D, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_Velocity.windup, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&gain_velocity, buffer, sizeof(float));
    buffer += sizeof(float);

    memcpy(&pid_angle.P, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_angle.I, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_angle.D, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_angle.windup, buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&gain_angle, buffer, sizeof(float));
    buffer += sizeof(float);
}