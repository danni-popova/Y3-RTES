/* Real-Time Embedded Systems
 *
 * School of Energy and Electronic Engineering
 * University of Portsmouth
 *
 * (c) Rinat Khusainov
 */

#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"

/*
 *  This function will be run by a separate task
 * It prints out "Hello world" message continuously with a delay of 3 seconds
 */
void Task2(void) {
    while (1) {
        printf("Hello world!\n");
        taskDelay(1 * sysClkRateGet());
    }
}

/* This function will be run by the main task
 * It starts a new task (Task 2) that executes function Task2
 * Then it waits for 20 seconds, deletes task 2, and exits itself
 */
void main(void) {
    int id;	/* To store the ID of the newly created task */

    /* Create a new task running function Task2 */
    id = taskSpawn("Task 2", 100, 0, 20000, (FUNCPTR)Task2, 0,0,0,0,0,0,0,0,0,0);

    /* Delay */
    taskDelay(10 * sysClkRateGet());

    /* Delete task 2 */
    taskDelete(id);

    printf("No new hello messages should appear now...\n");
    taskDelay(5 * sysClkRateGet());
}

