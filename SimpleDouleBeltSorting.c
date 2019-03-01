/* Author : Yordanka Popova up782716@myport.ac.uk */
#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "cinterface.h"
#include "wdLib.h"
#include "time.h"

/* Constants */
char LEFT = 1; 
char RIGHT = 0;

/* Counters */
int smallBlocks, bigBlocks, totalBlocks = 0;

/* Arrays to hold old and new sensors readings for both belts */
char newSensorReading[2] = { 0, 0 };
char oldSensorReading[2] = { 0, 0 };

/* Timer ID to control gates */
WDOG_ID openGatesTimerID;

WDOG_ID createTimer(void) /* Create a new timer for gate operation */
{
	WDOG_ID operateGateTimer;
	operateGateTimer = wdCreate();
	return operateGateTimer;
}

void openGates(void)
{
	setGates(0);
}

void closeGates(char belt)
{
	setGates(belt);
	wdStart(openGatesTimerID, 2 * sysClkRateGet(), (FUNCPTR)openGates, 0);
}

void PrintCounters(void)
{
	bigBlocks = totalBlocks - smallBlocks;
	printf("\nSmall blocks: %d\nBig blocks: %d\nTotal blocks: %d", smallBlocks, bigBlocks, totalBlocks);
}

void ReadSensor(unsigned char belt)
{
	/* Reset sensors - mandatory for reading */
	resetSizeSensors(belt);
	
	/* Read and save to new state */
	newSensorReading[belt] = readSizeSensors(belt);
	
	if(newSensorReading[belt] != oldSensorReading[belt])
	{		
		switch(newSensorReading[belt])
		{
			case 0 :
				if(oldSensorReading[belt] == 1)
					{
						smallBlocks++;
						wdStart(createTimer(), 3 * sysClkRateGet(), (FUNCPTR)closeGates, belt+1);
						PrintCounters();
					}
				break;
			case 1: 
				totalBlocks++;
				PrintCounters();
				break;
		}
		
		oldSensorReading[belt] = newSensorReading[belt];
	}
}

void run(void)
{
	startMotor();
	
	/* Set up timer to open gates */
	openGatesTimerID = wdCreate();
	
	while(1)
	{
		/* Read size sensors on LEFT belt*/ 
		ReadSensor(LEFT);
		
		/* Read size sensors on RIGHT belt */
		ReadSensor(RIGHT);
	}
}