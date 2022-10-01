#include "variables.h"

#define FLOAT_WRONG_INDEX(index) ((index > VAR_SYS_COUNT)||(index<0))

static float var_sys_float[VAR_SYS_COUNT];


void VAR_SetFloat(float value, VAR_float_e name)
{
    if(FLOAT_WRONG_INDEX(name))
        return;
    var_sys_float[name] = value;
}

float* VAR_GetFloatPtr(VAR_float_e name)
{
    if(FLOAT_WRONG_INDEX(name))
        return 0;
    return &var_sys_float[name];
}

float VAR_GetFloat(VAR_float_e name)
{
    if(FLOAT_WRONG_INDEX(name))
        return 0;
    return var_sys_float[name];
}