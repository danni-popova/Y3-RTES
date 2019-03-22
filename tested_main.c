#include "cinterface.h"
#include "stdio.h"
#include "vxWorks.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "stdlib.h"
#include "ioLib.h"
#include "wdLib.h"
#include "time.h"

int CheckSensor_id, Interface_id /*CheckEndSensor_id*/;
char GateState, MissCount;
char SmallCount[2] = {0, 0};
char LargeCount[2] = {0, 0};
char LargeCountSensor[2] = {0, 0};
char onBelt[2] = {0, 0};
SEM_ID CountSemID;
SEM_ID EndCountSemID;
SEM_ID CountSensorSemID;
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

void Interface(void){
  char c;
  while(1){
    c = getchar();
    switch(c){
       case 'q':
                printf("Shutting down... \n");
                taskDelete(CheckSensor_id);
                /*taskDelete(CheckEndSensor_id);*/
                stopMotor();
                printf("Goodbye! \n");
                taskDelete(Interface_id);
      case 'l':
    	  	  	if (MissCount > 0){
    	  	  		printf("Large block has been missed! \n");
    	  	  	}
                printf("Total Large blocks COUNTED on Conveyor 0 : %d \n", LargeCount[0]);
                break;
      case 's':
                printf("Total Small blocks DETECTED on Conveyor 0 : %d \n", SmallCount[0]);
                break;
      case 'o':
    	  	    if (MissCount > 0){
    	  	    	printf("Large block has been missed! \n");
    	      	}
                printf("Total Large blocks COUNTED on Conveyor 1 : %d \n", LargeCount[1]);
                break;
      case 'p':
                printf("Total Small blocks DETECTED on Conveyor 1 : %d \n", SmallCount[1]);
                break;
      case 'k':
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Large block count/detected value to 0 \n");
                LargeCount[0] = 0;
                LargeCount[1] = 0;
                semGive(CountSemID);
                semTake(EndCountSemID, WAIT_FOREVER);
                LargeCountSensor[0] = 0;
                LargeCountSensor[1] = 0;
                MissCount = 0;
                semGive(EndCountSemID);
                break;
      case 'a':
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Small block detected value to 0 \n");
                SmallCount[0] = 0;
                SmallCount[1] = 0;
                semGive(CountSemID);
                break;
      case 'z':
                printf("Large block DETECTED for conveyor 0: %d \n", LargeCountSensor[0]);
                break;
      case 'x':
                printf("Large block DETECTED for conveyor 1: %d \n", LargeCountSensor[1]);
                break;
      case 'h': printf("Welcome to block conveyor 4.0! \n"
    		  "Press: \n" "q to quit \n" "l to show number of large blocks counted \n"
    		  "s to show number of small blocks detected \n"
    		  "o to show number of large blocks counted \n"
    		  "p to show number of small blocks detected \n"
    		  "k to reset the large block counter \n" "a to reset the small block counter \n"
    		  "z to show number of large blocks detected on conveyor 0 \n"
    		  "x to show number of large blocks detected on conveyor 1 \n"
    		  "h to see this message again \n ");
      default: break;
    }

  }
}

 void CheckEndSensor(char belt){
    resetCountSensor(belt);
    char Count = readCountSensor(belt);
    if (belt == 0){
    	if (Count == 1){
    		semTake(EndCountSemID, WAIT_FOREVER);
        	LargeCountSensor[belt] ++;
        	semGive(EndCountSemID);
    	}
    	else{
    		semTake(EndCountSemID, WAIT_FOREVER);
    		MissCount ++;
    		semGive(EndCountSemID);
    	}
    }
    else if (belt == 1){
    	if (Count == 1){
    		semTake(EndCountSemID, WAIT_FOREVER);
    		LargeCountSensor[belt] ++;
    		semGive(EndCountSemID);
    	}
    	else{
    		semTake(EndCountSemID, WAIT_FOREVER);
    		MissCount ++;
    		semGive(EndCountSemID);
    	}
    }
}

void openGates(char belt){
	setGates(0);
	GateState = 0;
	onBelt[belt] = 0;
}

void closeGates(char belt){
    char NextState;
    onBelt[belt] ++;
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
        case 3 : if (onBelt[0] > 0){
        		 	 NextState = 3;
        		 	 setGates(3);
        		 }
        	     else{
        	    	 NextState = 2;
        	    	 setGates(2);
        	     }
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
        case 3 : if (onBelt[1] > 0){
        		 	 NextState = 3;
        		 	 setGates(3);
        		 }
        		 else{
        			 NextState = 1;
        			 setGates(1);
        		 }
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
      }
    }
    GateState = NextState;
    wdStart(openGatesTimerID, 1.5 * sysClkRateGet(), (FUNCPTR)openGates, belt);
}

void Analyse(CurrentState, LastState, belt){
  if (CurrentState != LastState){ /* New reading */
    switch(CurrentState){
      case 0 :
              if (LastState == 1){ /* First small block on belt*/
                printf("Close gate timer set for conveyor %d \n", belt);
                wdStart(createTimer(), 3 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount[belt] ++;
                semGive(CountSemID);
              }
              else if (LastState == 3){  /* Second small block on belt */
                printf("Second small block detected for conveyor %d! \n", belt);
                wdStart(createTimer(), 3 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount[belt] ++;
                semGive(CountSemID);
              }
              break;
      case 3 :
              if (LastState == 1){ /* Large block on belt */
            	wdStart(createCountTimer(), 5 * sysClkRateGet(), (FUNCPTR)CheckEndSensor, belt);
            	printf("Large block detected on conveyor %d! \n", belt);
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
    belt = 0;
    resetSizeSensors(0);
    CurrentState0 = readSizeSensors(0);
    Analyse(CurrentState0, LastState0, belt);
    LastState0 = CurrentState0;

    belt = 1;
    resetSizeSensors(1);
    CurrentState1 = readSizeSensors(1);
    Analyse(CurrentState1, LastState1, belt);
    LastState1 = CurrentState1;

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

  /*CheckEndSensor_id = taskSpawn("CheckEndSensor", 96, 0, 20000,
                      (FUNCPTR)CheckEndSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); */

  Interface_id = taskSpawn("Interface", 97, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  /* Set up timer to open gates */
  	openGatesTimerID = wdCreate();

  printf("Welcome to block conveyor 4.0! \n"
		  "Press: \n" "q to quit \n" "l to show number of large blocks counted \n"
		  "s to show number of small blocks detected \n"
		  "o to show number of large blocks counted \n"
		  "p to show number of small blocks detected \n"
		  "k to reset the large block counter \n" "a to reset the small block counter \n"
		  "z to show number of large blocks detected on conveyor 0 \n"
		  "x to show number of large blocks detected on conveyor 1 \n"
		  "h to see this message again \n ");
}
