Skip to content

Search or jump to…

Pull requests
Issues
Marketplace
Explore
 @c-flory Sign out
1
0 0 danni-popova/Y3-RTES
 Code  Issues 0  Pull requests 0  Projects 0  Wiki  Insights
Y3-RTES/OneGateOperation.c.txt
573f5ae  7 minutes ago
@danni-popova danni-popova Add old working project

162 lines (129 sloc)  2.81 KB
#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "cinterface.h"

int blocksDetected = 0;

int smallBlocksDetected = 0;
int bigBlocksDetected = 0;

char smallBlockFlag = 0;

void openGate(void)
{
	setGates(0);
}

void openGateTimer(void)
{
	int res;
	WDOG_ID openGateTimerID = wdCreate();

	if (openGateTimerID == NULL)
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}

	res = wdStart(openGateTimerID, 4 * sysClkRateGet(), (FUNCPTR)openGate, 0);

	if (res == ERROR)
	{
		printf("Cannot start the timer!!! Terminating...");
		exit(0);
	}
}

void closeGate(void)
{
	setGates(3);
}

void closeGateTimer(void)
{
	printf("Creating close gate timer");

	int res;
	WDOG_ID closeGateTimerID = wdCreate();

	if (closeGateTimerID == NULL)
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}

	res = wdStart(closeGateTimerID, 2 * sysClkRateGet(), (FUNCPTR)closeGate, 0);

	if (res == ERROR)
	{
		printf("Cannot start the timer!!! Terminating...");
		exit(0);
	}
}

void PrintStatus(void)
{
    while (1) {
    	printf("Total blocks detected: %d \n", blocksDetected);
    	printf("Small blocks detected: %d \n", smallBlocksDetected);

    	bigBlocksDetected = blocksDetected - smallBlocksDetected;

    	printf("Big blocks detected: %d \n", bigBlocksDetected);

        taskDelay(1 * sysClkRateGet());
    }
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
						{
							smallBlocksDetected++;
							closeGateTimer();
							openGateTimer();
						}
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

/*switch (command)
{
	case 0:
		printf("Opening both gates.");
		break;
	case 1:
		printf("Open gate 0, close gate 1");
		break;
	case 2:
		printf("Close gate 0, open gate 1");
		break;
	case 3:
		printf("Closing both gates.");
		break;
}
*/
