#pragma once
#include "time/time.h"

void CTRL_task(timeUs_t currentTime);
void CTRL_Init();

uint8_t* CTRL_SerializePID(uint8_t* len);
void CTRL_UpdatePID(uint8_t* buffer, uint8_t len);
void CTRL_NewHB();
void CTRL_StopMotors();
void CTRL_EnableMotors();
void CTRL_NewInput(uint8_t* data, uint8_t len);