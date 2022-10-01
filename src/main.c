#include <stm32g070xx.h>
#include "IO/LED.h"
#include "drivers/GPIO.h"
#include "config.h"
#include "Scheduler/scheduler.h"
#include "Tasks/tasks.h"
#include "time/time.h"
#include "setup.h"
#include "drivers/UART3.h"


void run();


int main()
{
    SetupSystem();
     GPIOSetPin(GPIOA, 10);
    
    DelayMs(1000);
   GPIOResetPin(GPIOA, 10);
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