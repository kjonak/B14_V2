#ifndef ODOMETRY_H
#define ODOMETRY_H

#include "time/time.h"





void ODO_Init(void);
void ODO_task(timeUs_t currentTime);
void ODO_ResetDist();
#endif