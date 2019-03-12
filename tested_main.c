#include "cinterface.h"
#include "stdio.h"
#include "vxWorks.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "stdlib.h"
#include "ioLib.h"
#include "wdLib.h"
#include "time.h"

char GateState;
char SmallCount0;
char SmallCount1;
char LargeCount0;
char LargeCount1;
char LargeCountSensor0;
char LargeCountSensor1;
SEM_ID MotorStateSemID;
SEM_ID CountSemID;
SEM_ID EndCountSemID;
SEM_ID CountSensorSemID;


void Interface(void){
  char c;
  while(1){
    c = getchar();
    switch(c){
      /* case 'q':
                printf("Shutting down... \n");
                taskDelete(CheckSensor_id);
                taskDelete(MotorController0_id);
                taskDelete(MotorController1_id);
                printf("Goodbye! \n");
                taskDelete(Interface_id); */
      case 'l':
                printf("Total Large blocks counted: %d \n", LargeCount0);
                break;
      case 's':
                printf("Total Small blocks detected: %d \n", SmallCount0);
                break;
      case 'o':
                printf("Total Large blocks counted: %d \n", LargeCount1);
                break;
      case 'p':
                printf("Total Small blocks detected: %d \n", SmallCount1);
                break;
      case 'k':
                semTake(EndCountSemID, WAIT_FOREVER);
                printf("Reset Large block count value to 0 \n");
                LargeCount0 = 0;
                LargeCount1 = 0;
                LargeCountSensor0 = 0;
                LargeCountSensor1 = 0;
                semGive(CountSemID);
                break;
      case 'a':
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Small block detected value to 0 \n");
                SmallCount0 = 0;
                SmallCount1 = 0;
                semGive(CountSemID);
                break;
      case 'z':
                printf("Large block count for conveyor 0: %d \n", LargeCountSensor0);
                break;
      case 'x':
                printf("Large block count for conveyor 1: %d \n", LargeCountSensor1);
                break;
/*      case 'h': printf("Welcome to block conveyor 4.0! \n
                Press: \n
                q to quit \n
                l to show number of large blocks counted \n
                s to show number of small blocks detected \n
                o to show number of small blocks counted \n
                p to show number of small blocks detected \n
                k to reset the large block counter \n
                a to reset the small block counter \n
                z to show number of large blocks detected on conveyor 0 \n
                x to show number of large blocks detected on conveyor 1 \n
                h to see this message again \n");
              break; */
      default: break;
    }

  }
}

void CheckEndSensor(void){
  while(1){
    semTake(CountSensorSemID, WAIT_FOREVER);
    taskDelay(1 * sysClkRateGet());
    resetCountSensor(0);
    resetCountSensor(1);
    printf("Reading count 0 sensor...\n");
    char Count0 = readCountSensor(0);
    char Count1 = readCountSensor(1);
    if (Count0 == 1){
      semTake(EndCountSemID, WAIT_FOREVER);
      LargeCountSensor0 ++;
      semGive(EndCountSemID);
    }
    else if (Count1 == 1){
      semTake(EndCountSemID, WAIT_FOREVER);
      LargeCountSensor1 ++;
      semGive(EndCountSemID);
    }
  }
}

void openGates(belt){
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (belt == 1){
      switch (CheckGateState){
        case 0 : NextState = 0;
                 setGates(0);
                 break;
        case 1 : NextState = 1;
                 setGates(1);
                 break;
        case 2 : NextState = 0;
                 setGates(0);
                 break;
        case 3 : NextState = 1;
                 setGates(1);
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
        }
    }
    else if (belt == 0){
      switch (CheckGateState){
        case 0 : NextState = 0;
                 setGates(0);
                 break;
        case 1 : NextState = 0;
                 setGates(0);
                 break;
        case 2 : NextState = 2;
                 setGates(2);
                 break;
        case 3 : NextState = 2;
                 setGates(2);
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
      }
    }
  printf("Motor 0 state is %d \n", NextState);
  GateState = NextState;
  semGive(MotorStateSemID);
  }
}

void closeGates(belt){
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (belt == 1){
      switch (CheckGateState){
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
      switch (CheckGateState){
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
    printf("Motor 1 state is %d \n", NextState);
    GateState = NextState;
    semGive(MotorStateSemID);
    wdStart(createTimer, 2 * sysClkRateGet(), (FUNCPTR)openGates, belt);
    semGive(CountSensorSemID);
  }
}
}

WDOG_ID createTimer(void) /* Create a new timer for gate operation */
{
	WDOG_ID operateGateTimer;
	operateGateTimer = wdCreate();
	return operateGateTimer;
}

void Analyse(CurrentState, LastState, belt){
  if (CurrentState != LastState){ /* New reading */
    switch(CurrentState){
      case 0 :
              if (LastState == 1){ /* First small block on belt*/
                printf("Close gate timer set for conveyor %d \n", belt);
                wdStart(createTimer(), 3 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount1 ++;
                semGive(CountSemID);
              }
              else if (LastState == 3){  /* Second small block on belt */
                printf("Second small block detected for conveyor %d! \n", belt);
                wdStart(createTimer(), 3 * sysClkRateGet(), (FUNCPTR)closeGates, belt);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount1 ++;
                semGive(CountSemID);
              }
              break;
      case 3 :
              if (LastState == 1){ /* Large block on belt */
                semTake(CountSemID, WAIT_FOREVER);
                printf("Large block detected on conveyor %d! \n", belt);
                LargeCount1 ++;
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
  belt = 0;
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

    /* taskDelay(0.25 * sysClkRateGet()); */
  }
}

int main(void){
  startMotor();
  int CheckSensor_id;
  int Interface_id;
  int CheckEndSensor0_id;
  int CheckEndSensor1_id;

  MotorStateSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (MotorStateSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
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
  CountSensorSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (CountSensorSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }

  /* Set up tasks*/
  CheckSensor_id = taskSpawn("CheckSensor", 95, 0, 20000,
                      (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  CheckEndSensor0_id = taskSpawn("CheckEndSensor0", 96, 0, 20000,
                      (FUNCPTR)CheckEndSensor0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  CheckEndSensor1_id = taskSpawn("CheckEndSensor1", 96, 0, 20000,
                      (FUNCPTR)CheckEndSensor1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  Interface_id = taskSpawn("Interface", 97, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  /*printf("Welcome to block conveyor 4.0! \n" "Press: \n" q to quit \n l to show number of large blocks counted \n s to show number of small blocks detected \n o to show number of small blocks counted \n p to show number of small blocks detected \n k to reset the large block counter \n a to reset the small block counter \n z to show number of large blocks detected on conveyor 0 \n x to show number of large blocks detected on conveyor 1 \n h to see this message again \n ");*/
}

/* Motor control 1 UP timer does not execute - why? Check timers */
/* Blocktimepointer1 does not reset */
/* open gate timer executed too many times */
/* Count sensor no longer sensing */
/* timers not signalling properly to gates */
/* consider priorities */
/* reads first block but gates dont work on any subsequent blocks */
/* Create seperate semaphores for blocktimepointers to avoid timing collision? */

/* Create timers individually in a smarter way */
/* open gate timer set after close gate timer executes */
/* Remove blockpointer */
