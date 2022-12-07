set(MAIN_SRC

main.c
setup.c
Variables.c
time/time.c

drivers/GPIO.c
drivers/UART1.c
drivers/USART.c

ControlTask/CTRL_Task.c
ControlTask/PID.c
ControlTask/odometry.c

Scheduler/scheduler.c
Tasks/tasks.c

Comunication/com_protocol.c
Comunication/Telemetry.c
Comunication/CommunicationHandler.c

IO/LED.c
IO/Motors.c

PARENT_SCOPE
)