#include <stm32g070xx.h>
#include "IO/LED.h"
#include "drivers/GPIO.h"
#include "IO/Motors.h"
#include "config.h"
#include "Scheduler/scheduler.h"
#include "Tasks/tasks.h"
#include "time/time.h"
#include "setup.h"
#include "drivers/UART1.h"
#include "Comunication/Telemetry.h"

void run();
uint8_t cnt = 0;




int main()
{
    SetupSystem();
    MOTORS_Enable();
    MOTORS_SetOutputs(0,0);
    while(1)
    {
       run();
    }

}



void run()
{
    initTasks();

    while(1)
    {
        scheduler();
    }
}