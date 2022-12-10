#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>

typedef enum
{
    VAR_VELOCITY,
    VAR_HEADING,
    VAR_ANGULAR_VELOCITY,
    VAR_DISTANCE,

    VAR_DESIRED_VELOCITY,
    VAR_DESIRED_HEADING,
    VAR_DESIRED_ANGULAR_VELOCITY,
    

    VAR_SYS_COUNT
}VAR_float_e;

void VAR_SetFloat(float value, VAR_float_e name);
float *VAR_GetFloatPtr(VAR_float_e name);
float VAR_GetFloat(VAR_float_e name);


#endif