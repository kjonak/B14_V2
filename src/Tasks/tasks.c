#include <stdbool.h>
#include <stdio.h>
#include "Scheduler/scheduler.h"
#include "config.h"
#include "IO/LED.h"
#include "tasks_config.h"

#include "ControlTask/odometry.h"
#include "ControlTask/CTRL_Task.h"
#include "Comunication/Telemetry.h"
#include "Comunication/CommunicationHandler.h"

task_t tasks[TASK_COUNT] = {
	[TASK_SYSTEM] =
		{
			.taskName = "SYSTEM",
			.taskFun = taskSystem,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_SYSTEM_HZ),
			.staticPriority = TASK_PRIORITY_LOW},
	[TASK_ODOMETRY] =
		{
			.taskName = "TASK_ODOMETRY",
			.taskFun = ODO_task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_ODOMETRY_HZ),
			.staticPriority = TASK_PRIORITY_REALTIME},
	[TASK_CTRL] =
		{
			.taskName = "TASK_CTRL",
			.taskFun = CTRL_task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_CTRL_HZ),
			.staticPriority = TASK_PRIORITY_REALTIME},
	[TASK_TELEMETRY] =
		{
			.taskName = "TASK_TELEMETRY",
			.taskFun = TELEMETRY_task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_TELEMETRY_HZ),
			.staticPriority = TASK_PRIORITY_IDLE},
	[TASK_COMMHANDLER] =
		{
			.taskName = "TASK_CTRL_RECEIVER",
			.taskFun = COMHANDLER_Task,
			.checkFun = COMHANDLER_CheckFun,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_COMMHANDLER_HZ),
			.staticPriority = TASK_PRIORITY_IDLE},

};

void initTasks(void)
{
	ODO_Init();
	CTRL_Init();
	TELEMETRY_init();
	COMHANDLER_Init();
	initScheduler();
	enableTask(TASK_ODOMETRY, true);
	enableTask(TASK_CTRL,true);
	enableTask(TASK_TELEMETRY, true);
	enableTask(TASK_COMMHANDLER, true);
}