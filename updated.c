#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "cinterface.h"
#include "wdLib.h"
#include "time.h"

int blocksDetected = 0;
int smallBlocksDetected = 0;
int bigBlocksDetected = 0;

/* IDs to control gates tasks, timers, semaphores */
SEM_ID gate_control_sem_id;
WDOG_ID gate_control_timer_id; 
int gate_control_task_id;
int gate_sem_task_id;

void timerCallback(void)
{
	semGive(gate_control_sem_id);
}

void ReadSensorsPeriodicTask(void)/* Periodic task */
{
	char newMainState = 0; 
	char oldMainState = 0;
		
	while(1)
	{
		resetSizeSensors(0);
		newMainState = readSizeSensors(0);
		
		if(newMainState != oldMainState)
		{
			switch(newMainState)
			{
				case 0 : 
					printf("\nNo blocks in front of size sensors\n");
					
					if(oldMainState == 1)
					{
						wdStart(gate_control_timer_id, 3 * sysClkRateGet(), (FUNCPTR)timerCallback, 0);
						printf("Small block detected, start timer.");
						smallBlocksDetected++;
					}
					break;
					
				case 1:
					printf("\nBlock in front of first size sensor\n");
					blocksDetected++;
					break;
			}
			
			oldMainState = newMainState; 
		}
	}
}

void ControlGatesTask(void)
{
	while(1)
	{
		printf("Waiting for sensor semaphore");
		semTake(gate_control_sem_id, WAIT_FOREVER);
		
		printf("\nReceived semaphore.");
		setGates(3);
		
		taskDelay(2 * sysClkRateGet());
		setGates(0);
	}
}


void main(void)
{
	printf("\n New run.\nStopping motor");
	stopMotor();
	taskDelay(5 * sysClkRateGet());
	
	printf("\nReady for execution.");
	startMotor();
	
	/* Create semaphore */
	gate_control_sem_id = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	
	/* Create timer */
	gate_control_timer_id = wdCreate();
	
	/* Run task to read sensors */
	gate_control_task_id = taskSpawn("ReadSensorsPeriodicTask", 100, 0, 20000, (FUNCPTR)ReadSensorsPeriodicTask, 0,0,0,0,0,0,0,0,0,0);
	
	/* Run task waiting for semaphore*/
	gate_sem_task_id = taskSpawn("WaitGate", 99, 0, 20000, (FUNCPTR)ControlGatesTask, 0,0,0,0,0,0,0,0,0,0);
}
