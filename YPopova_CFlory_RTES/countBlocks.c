#include "cinterface.h"
#include "time.h"
#include "vxWorks.h"
#include "taskLib.h"
#include "wdLib.h"
#include "stdlib.h"
#include "stdio.h"

/* Timer ID */

int blocksDetected = 0;

int smallBlocksDetected = 0;
int bigBlocksDetected = 0;


void PrintStatus(void) 
{
    while (1) 
    {
    	bigBlocksDetected = blocksDetected - smallBlocksDetected;
    	
    	printf("Total blocks detected: %d \n", blocksDetected);
    	printf("Small blocks detected: %d \n", smallBlocksDetected);
    	printf("Big blocks detected: %d \n", bigBlocksDetected);
    	
        taskDelay(2 * sysClkRateGet());
    }
}

void operateGate(void)
{
	printf("Closing both gates");
	
	setGates(3);

	printf("Waiting 1 second");
	
	taskDelay(1 * sysClkRateGet());
	
	printf("Opening both gates");
	
	setGates(0);
}

void readMainSensors(void) 
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
					printf("No blocks in front of size sensors\n");
					
					if(oldMainState == 1)
						smallBlocksDetected++;
					
					break;
				case 1:
					printf("Block in front of first size sensor\n");
					blocksDetected++;
					break;
				case 2:
					printf("Block in front of second size sensor\n");
					break;
				case 3:
					printf("Block(s) in front of both size sensors\n");
					break;
			}
			
			oldMainState = newMainState; 
		}
	}
}

void main(void) 
{		
	startMotor();
	
	WDOG_ID timerID;	
		
	/* Create the watchdog timer */
	timerID = wdCreate();

	if (timerID == NULL) 
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}
	
	int res = wdStart(timerID, 3 * sysClkRateGet(), (FUNCPTR)operateGate, 0);
							
	if (res == ERROR) 
		printf("Cannot start the timer!!! Terminating...");
	
	int readMainSensorsId;	
	readMainSensorsId = taskSpawn("ReadingMainSensors", 100, 0, 20000, (FUNCPTR)readMainSensors, 0,0,0,0,0,0,0,0,0,0);

	int printStatusId;
	printStatusId = taskSpawn("PrintingBlocksCount", 99, 0, 20000, (FUNCPTR)PrintStatus, 0,0,0,0,0,0,0,0,0,0); 
	
	taskDelay(10 * sysClkRateGet());

	taskDelete(readMainSensorsId);
	taskDelete(printStatusId);

	stopMotor();

	exit(0);
}

void stop(void)
{
	stopMotor();
}

void controlGates(char command) 
{
	switch (command) 
	{
		case 0:
			printf("Opening both gates.");
			break;
		case 1:
			printf("Opening gate 0, close gate 1.");
			break;
		case 2:
			printf("Closing gate 0, open gate 1.");
			break;
		case 3:
			printf("Closing both gates.");
			break;
	}

	setGates(command);
}
