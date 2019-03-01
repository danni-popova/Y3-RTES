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

/* Arrays to hold old and new sensors readings for both belts */
char newSensorReading[2] = { 0, 0 };
char oldSensorReading[2] = { 0, 0 };

/* Timer ID to control gates */
WDOG_ID closeGatesTimerID;
WDOG_ID openGatesTimerID;

void openGates(void)
{
	setGates(0);
}

void closeGates(void)
{
	setGates(3);
	wdStart(openGatesTimerID, 1.5 * sysClkRateGet(), (FUNCPTR)openGates, 0);
}

void ReadSensor(unsigned char belt)
{
	/* Reset sensors - mandatory for reading */
	resetSizeSensors(belt);
	
	/* Read and save to new state */
	newSensorReading[belt] = readSizeSensors(belt);
	
	if(newSensorReading[belt] != oldSensorReading[belt])
	{
		printf("\nNew state on belt %c", belt);
		
		switch(newSensorReading[belt])
		{
			case 0 :
				printf("\nNo blocks in front of size sensors on belt %c", belt);
				
				if(oldSensorReading[belt] == 1)
					printf("Small block detected.");
				
				/* To start timer */
				wdStart(closeGatesTimerID, 2 * sysClkRateGet(), (FUNCPTR)closeGates, 0);
				
				break;
			case 1: 
				printf("\nBlock in front of first size belt on belt %c", belt);
				break;
			case 2:
				printf("\nBlock in front of second size belt on belt %c", belt);
				break;
			case 3:
				printf("\nBlock(s) in front of both size sensors on belt %c", belt);
				break;
		}
		
		oldSensorReading[belt] = newSensorReading[belt];
	}
}

void run(void)
{
	startMotor();
	
	/* Set up timer to close gates */
	closeGatesTimerID = wdCreate();
	
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

