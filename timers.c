/* Real-Time Embedded Systems
 *
 * School of Energy and Electronic Engineering
 * University of Portsmouth
 *
 * (c) Rinat Khusainov
 */

#include "vxWorks.h"
#include "taskLib.h"
#include "wdLib.h"
#include "stdlib.h"
#include "stdio.h"

char flag = 0;		/* Flag telling when the timer goes off */
WDOG_ID timerID;	/* Timer ID */


/* This is the timer callback function
 * It gets called when the timer expires 
 */
void timerCallback(void) {
    /* Set the flag */
    flag = 1;
}


/* This function is executed by the main task
 * It sets the timer, then check the value of the flag once per second
 * to see when the timer expires and prints the corresponding message
 */
void main(void) {
    int res;

    /* Create the watchdog timer */
    timerID = wdCreate();

    if (timerID == NULL) {
        printf("Cannot create timer!!! Terminating this task...");
        exit(0);
    }

    /* Start the timer to expire in 10 seconds and call function "timerCallback" */
    res = wdStart(timerID, 10 * sysClkRateGet(), (FUNCPTR)timerCallback, 0);

    if (res == ERROR) {
        printf("Cannot start the timer!!! Terminating...");
        exit(0);
    }

    /* Check the flag value once per second */
    while (1) {
        printf("Checking the flag value...");
        if (flag != 0) {
           printf("Timer expired!!!\n");
           break;
        }
        printf("Still waiting for the timer\n");
        taskDelay(1 * sysClkRateGet());
    }   
}

