#include "vxWorks.h"
#include "taskLib.h"
#include "wdLib.h"
#include "stdlib.h"
#include "stdio.h"

char flag = 0;		/* Flag telling when the timer goes off */
WDOG_ID timerID;	/* Timer ID */

void timerCallback(void) 
{
	printf("Timer function called.\n");
    flag = 1;
}

void createTimer(void)
{
	int res;
	timerID = wdCreate();

	if (timerID == NULL) 
	{
		printf("Cannot create timer!!! Terminating this task...");
		exit(0);
	}
	
	res = wdStart(timerID, 10 * sysClkRateGet(), (FUNCPTR)timerCallback, 0);

	if (res == ERROR) 
	{
		printf("Cannot start the timer!!! Terminating...");
		exit(0);
	}
}

void main(void) 
{
    createTimer();
    
    while (1) 
    {
        printf("Checking the flag value...");
        
        if (flag != 0) 
        {
           printf("Timer expired!!!\n");
           break;
        }
        
        printf("Still waiting for the timer\n");
        taskDelay(2 * sysClkRateGet());
    }
}