#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "cinterface.h"

char smallBlocksCount = 0;
char bigBlocksCount = 0; 
char blocksCount = 0;

char gatesState = 0;
char leftGateFlag = 0;
char rightGateFlag = 0;

int readLeftSensorsTaskId, readRightSensorsTaskId, operateGatesTaskId; 

void main(void)
{
	createTasks();
}

void createTasks(void)
{
    // Tasks for reading sensors on each belt
    readLeftSensorsTaskId = taskSpawn("ReadLeftSensors",
         100, 0, 20000, (FUNCPTR)readLeftSizeSensors, 0,0,0,0,0,0,0,0,0,0);

    readLeftSensorsTaskId = taskSpawn("ReadRightSensors",
         101, 0, 20000, (FUNCPTR)readRightSizeSensors, 0,0,0,0,0,0,0,0,0,0);
    
    // Task for operating gates
    operateGatesTaskId = taskSpawn("OperateGates",
         99, 0, 20000, (FUNCPTR)OperateGates, 0,0,0,0,0,0,0,0,0,0);
}

void readLeftSizeSensors(void)
{
    char newState = 0; 
    char oldState = 0;

    // Continually reset and read sensors, operaing only upon change of state
    while(1)
    {
        resetSizeSensors(0);
        newState = readSizeSensors(0);

        // If the readinng is new, establish current block size
        if(newState != oldState)
        {

            if(newState == 0) && (oldState == 1)
            {
                printf("Small block on left belt");
                createCloseGateTimer(0);
            }
            
            // Reset old state to current state
            oldState = newState;
        }
    }
}

void readRightSizeSensors(void)
{
    char newState = 0; 
    char oldState = 0;

    // Continually reset and read sensors, operaing only upon change of state
    while(1)
    {
        resetSizeSensors(1);
        newState = readSizeSensors(1);

        // If the readinng is new, establish current block size
        if(newState != oldState)
        {

            if(newState == 0) && (oldState == 1)
            {
                printf("Small block on right belt");
                createCloseGateTimer(1);
            }
            
            // Reset old state to current state
            oldState = newState;
        }
    }
}

void createCloseGateTimer(char gate)
{
    int res;
	WDOG_ID clearGatesFlagTimer = wdCreate();

	if (clearGatesFlagTimer == NULL) 
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}
	
	res = wdStart(clearGatesFlagTimer,
                  1 * sysClkRateGet(),
                  (FUNCPTR)setGateFlag,
                  gate);

	if (res == ERROR) 
	{
		printf("Cannot start the timer!!! Terminating...");
		exit(0);
	}
}

void setGateFlag(char gate)
{
    if(gate == 0)
    {
        leftGateFlag = 1;
    }
    else if(gate == 1)
    {
        rightGateFlag = 1;
    }
}

void OperateGates(void)
{
    while(1)
    {
        // Wait for gates flag to be raised
        
        if(leftGateFlag == 0 && rightGateFlag == 0)
        {
            // Open both gates
            setGates(0);
        }
        if(leftGateFlag == 0 && rightGateFlag == 1)
        {
            // Open left, close right
            setGates(1);
        }
        if(leftGateFlag == 1 && rightGateFlag == 0)
        {
            // Close left, open right
            setGates(2);
        }
        if(leftGateFlag == 1 && rightGateFlag == 1)
        {
            // Open both
            setGates(3);
        }

        createGateFlagResetTimer(leftGateFlag, rightGateFlag);
    }    
}

void createGateFlagResetTimer(char lGateState, char rGateState)
{
    int res;
	WDOG_ID clearGatesFlagTimer = wdCreate();

	if (clearGatesFlagTimer == NULL) 
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}
	
	res = wdStart(clearGatesFlagTimer,
                  1 * sysClkRateGet(),
                  (FUNCPTR)openGate,
                  lGateState, rGateState);

	if (res == ERROR) 
	{
		printf("Cannot start the timer!!! Terminating...");
		exit(0);
	}
}

void clearFlags(char lGateState, chatrGateState)
{
    if(lGateState == 1)
        leftGateFlag = 0;

    if(rGateState == 1)
        rightGateFlag == 0;
}

/*

Truth table for gates: 

LeftFlag    RightFlag   SetGates
0           0           0 - open both
0           1           1 - open left, close right
1           0           2 - close left, open right
1           1           3 - open both

*/