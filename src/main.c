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
#include "Variables.h"
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
    timeMs_t t = 0;
    initTasks();

    while(1)
    {
        scheduler();
        // if((micros()-t)>1000000)
        // {
        //     float heading = VAR_GetFloat(VAR_DESIRED_HEADING)-20.f;
        //     if (heading>=360.f)
        //         {
        //             heading -= 360.;
        //         }
        //     if(heading<0.f)
        //         heading+=360.;
        //     t = micros();
        //     VAR_SetFloat(heading, VAR_DESIRED_HEADING);
             
        // }
       
    }
}