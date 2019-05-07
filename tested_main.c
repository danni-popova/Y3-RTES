#include "cinterface.h"
#include "stdio.h"
#include "vxWorks.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "stdlib.h"
#include "ioLib.h"
#include "wdLib.h"
#include "time.h"

int CheckSensor_id, Interface_id;

/* Varibale to check and update state of gates */
char GateState;

/* Variables to hold number of detected blocks */
char SmallCount[2] = {0, 0};
char LargeCount[2] = {0, 0};
char LargeCountSensor[2] = {0, 0};

/* Semaphores for access control of global variables */
SEM_ID CountSemID;
SEM_ID EndCountSemID;
SEM_ID CountSensorSemID;

/* Timer ID for opening gates */
WDOG_ID openGatesTimerID;

WDOG_ID createTimer(void){ /* Create a new timer for close gate operation */
	WDOG_ID operateGateTimer;
	operateGateTimer = wdCreate();
	return operateGateTimer;
}

WDOG_ID createCountTimer(void){ /* Create a new timer for close gate operation */
	WDOG_ID operateCountTimer;
	operateCountTimer = wdCreate();
	return operateCountTimer;
}

void Interface(void){ /* Single key press interface */
  char c;
  char TotalDetected;
  char TotalCounted;
  char Missed;

  while(1){
    c = getchar();

    /* Calculate total number of blocks */
    TotalDetected = LargeCountSensor[0] + LargeCountSensor[1];
    TotalCounted = LargeCount[0] + LargeCount[1];
    Missed = TotalCounted - TotalDetected; /* Missed updates */
    char pause;

    switch(c){
       case 'q':
                printf("\n" "Shutting down... \n");
                taskDelete(CheckSensor_id);
                semDelete(CountSemID);
                semDelete(EndCountSemID);
                semDelete(CountSensorSemID);
                stopMotor();
                printf("\n" "Goodbye! \n");
                taskDelete(Interface_id);
                break;
       case 'c':
    	  	  	if (Missed > 0){
    	  	  		printf("\n" "ERROR : Large block has been missed! \n");
    	  	  	}
                printf("Large blocks COUNTED on Conveyor 0 : %d \n", LargeCount[0]);
                break;
       case 'v':
                printf("Small blocks DETECTED on Conveyor 0 : %d \n", SmallCount[0]);
                break;
       case 'b':
    	  	    if (Missed > 0){
    	  	    	printf("ERROR : Large block has been missed! \n");
    	      	}
                printf("Large blocks COUNTED on Conveyor 1 : %d \n", LargeCount[1]);
                break;
       case 'n':
                printf("Small blocks DETECTED on Conveyor 1 : %d \n", SmallCount[1]);
                break;
       case 'w':
    	   	   /* Reset large block counter before gates */
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Large block count/detected value to 0 \n");
                LargeCount[0] = 0;
                LargeCount[1] = 0;
                semGive(CountSemID);

                /* Reset large block counter after gates */
                semTake(EndCountSemID, WAIT_FOREVER);
                LargeCountSensor[0] = 0;
                LargeCountSensor[1] = 0;
                semGive(EndCountSemID);

                break;
       case 'e':
    	   	   /* Reset small block counter before gates */
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Small block detected value to 0 \n");
                SmallCount[0] = 0;
                SmallCount[1] = 0;
                semGive(CountSemID);
                break;
       case 'z':
                printf("Large blocks DETECTED for conveyor 0: %d \n", LargeCountSensor[0]);
                break;
       case 'x':
                printf("Large blocks DETECTED for conveyor 1: %d \n", LargeCountSensor[1]);
                break;
       case 'l':
                printf("\n" "TOTAL Large blocks : %d \n", TotalDetected);
				if (Missed > 0){
                	printf("TOTAL Large blocks MISSED : %d \n", Missed);
                }
                break;
       case 't':
    	   	    if (pause == 0){
    	   	    	stopMotor();
    	   	    	printf("Belt paused! \n");
    	   	    	pause = 1;
    	   	    }
    	   	    else if (pause == 1){
    	   	    	startMotor();
    	   	    	printf("Belt ON \n");
    	   	    	pause = 0;
    	   	    }
    	   	    break;
       case 'i':
    	   	    printf("Checking gate operation, Please wait... \n");
    	   	    setGates(1);
    	   	    taskDelay(1 * sysClkRateGet());
    	   	    setGates(2);
    	   	    taskDelay(1 * sysClkRateGet());
    	   	    setGates(3);
    	   	    taskDelay(1 * sysClkRateGet());
    	   	    setGates(0);
    	   	    taskDelay(1 * sysClkRateGet());
    	   	    printf("Test complete! \n");
    	   	    break;

       case 'h': printf("--------------------------------------------------------------- \n"
    		  "Welcome to block conveyor 4.0! \n"
    		  "Press: \n"
    		  "q to quit \n"
    		  "l to show TOTAL large blocks detected and missed \n"
    		  "c to show number of large blocks COUNTED on Conveyor 0 \n"
    		  "v to show number of small blocks DETECTED on Conveyor 0 \n"
    		  "b to show number of large blocks COUNTED on Conveyor 1 \n"
    		  "n to show number of small blocks DETECTED on Conveyor 1 \n"
    		  "w to reset the large block counter \n"
    		  "e to reset the small block counter \n"
    		  "z to show number of large blocks detected on conveyor 0 \n"
    		  "x to show number of large blocks detected on conveyor 1 \n"
    		  "t to pause/start the belt \n"
    		  "i to test the gate control \n"
    		  "h to see this message again \n "
    		  "--------------------------------------------------------------- \n");
      default: break;
    }

  }
}

 void CheckEndSensor(char belt){ /* Timer callback to read count sensor */

	resetCountSensor(belt);
    char Count = readCountSensor(belt);
    if (Count == 1){
    	semTake(EndCountSemID, WAIT_FOREVER);
    	LargeCountSensor[belt] ++;
    	semGive(EndCountSemID);
    }
}

void openGates(char belt){	/* Timer callback to open gates after small block */
	setGates(0);
	GateState = 0;
}

void closeGates(char belt){ /* Timer callback to control gates */
    char NextState;
        if (belt == 1){
          switch (GateState){
            case 0 : NextState = 2;
                     setGates(2);
                     break;
            case 1 : NextState = 3;
                     setGates(3);
                     break;
            case 2 : NextState = 2;
                     setGates(2);
                     break;
            case 3 : NextState = 3;
            		 setGates(3);
                     break;
            default : NextState = 0;
                      setGates(0);
                     break;
          }
        }
        else if (belt == 0){
          switch (GateState){
            case 0 : NextState = 1;
                     setGates(1);
                     break;
            case 1 : NextState = 1;
                     setGates(1);
                     break;
            case 2 : NextState = 3;
                     setGates(3);
                     break;
            case 3 : NextState = 3;
            		 setGates(3);
                     break;
            default : NextState = 0;
                      setGates(0);
                      break;
          }
        }
	/* Update state */
    GateState = NextState;

    /* Start countdown to re-open gates */
    wdStart(openGatesTimerID, 1.7 * sysClkRateGet(), (FUNCPTR)openGates, belt);
}

void Analyse(CurrentState, LastState, belt){
  if (CurrentState != LastState){ /* New reading */
    switch(CurrentState){
      case 0 :
              if (LastState == 1){ /* First small block on belt*/
            	printf("\n" "Small block detected for conveyor %d! \n", belt);
                wdStart(createTimer(), 2.8 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount[belt] ++;
                semGive(CountSemID);
              }
              else if (LastState == 3){  /* Second small block on belt */
                printf("\n" "Second small block detected for conveyor %d! \n", belt);
                wdStart(createTimer(), 2.8 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount[belt] ++;
                semGive(CountSemID);
              }
              break;
      case 3 :
              if (LastState == 1){ /* Large block on belt */
            	wdStart(createCountTimer(), 4.5 * sysClkRateGet(), (FUNCPTR)CheckEndSensor, belt);
            	printf("\n" "Large block detected on conveyor %d! \n", belt);

              	/* Update large block counter */
                semTake(CountSemID, WAIT_FOREVER);
                LargeCount[belt] ++;
                semGive(CountSemID);
              }
              break;
      default :
               break;
    }
  }
}

void CheckSensor(){
  char CurrentState0 = 0;
  char LastState0 = 0;
  char CurrentState1 = 0;
  char LastState1 = 0;
  char belt = 0;

  while(1){
	  /* Read sensors on belt 0 */
    belt = 0;
    resetSizeSensors(0);
    CurrentState0 = readSizeSensors(0);
    Analyse(CurrentState0, LastState0, belt);
    LastState0 = CurrentState0;
    /* Read sensors on belt 1 */
    belt = 1;
    resetSizeSensors(1);
    CurrentState1 = readSizeSensors(1);
    Analyse(CurrentState1, LastState1, belt);
    LastState1 = CurrentState1;

    /* Delay task slightly to allow other tasks to run */
    taskDelay(0.05 * sysClkRateGet());


  }
}

int main(void){
  startMotor();

  CountSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (CountSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  EndCountSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (EndCountSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }

  /* Set up tasks*/
  CheckSensor_id = taskSpawn("CheckSensor", 95, 0, 20000,
                      (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  Interface_id = taskSpawn("Interface", 97, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  /* Set up timer to open gates */
  	openGatesTimerID = wdCreate();

  printf("Welcome to block conveyor 4.0! \n"
		  "Press: \n"
		  "q to quit \n"
		  "l to show TOTAL large blocks detected and missed \n"
		  "c to show number of large blocks counted \n"
		  "v to show number of small blocks detected \n"
		  "b to show number of large blocks counted \n"
		  "n to show number of small blocks detected \n"
		  "w to reset the large block counter \n"
		  "e to reset the small block counter \n"
		  "z to show number of large blocks detected on conveyor 0 \n"
		  "x to show number of large blocks detected on conveyor 1 \n"
		  "t to pause/start the belt \n"
		  "i to test the gate control \n"
		  "h to see this message again \n "
		  "--------------------------------------------------------------- \n");
}
