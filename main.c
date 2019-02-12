// Catherine Flory && Danni Popova
// Real Time Embedded Systems

// Code that handles two conveyer belts moving large and small blocks. the
// software should be able to detect and count large and small blocks, open a
// gate for a large block and close a gate for a small block.
// The software should retain a seperate count for: small objects detected,
// large objects detected, and large objects collected.
// The software should have a text based interface that allows the user to:
// display number of small objects detected, number of large objects detected,
// number of large objects collected, reset the counter values for each
// conveyor, and shutdown the application and free up any memory used by it.

#include <cinterface.h>
#include <stdio.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <stdlib.h>
#include <ioLib.h>
#include "time.h"

#define UP 1
#define DOWN 0


char BlockTimeQueue0[4]; // Put system time elapsed in this array to reset WD timers if there are several blocks in queue
char BlockTimeQueue1[4];

char GateState; // Check and Set GateState before execution (after timer)
char SmallCount0;
char SmallCount1;
char LargeCount0;
char LargeCount1;
char LargeCountSensor0;
char LargeCountSensor1;
WD_ID timer_T1_ID; // Opening timer for conveyor 0
WD_ID timer_T2_ID; // Opening timer for conveyor 1
WD_ID timer_T3_ID; // Closing timer for conveyor 0
WD_ID timer_T4_ID; // Closing timer for conveyor 1
MSG_Q_ID queueMotorC0ID;
MSG_Q_ID queueMotorC1ID;
SEM_ID MotorStateSemID;
SEM_ID CountSemID;
SEM_ID EndCountSemID;

///////////////////////////// User Input ////////////////////////////

void Interface(void){
  char msgCount;
  char SensorCount = 0;
  char c;
  while(1){
    // Shutdown - end tasks
    // Restart
    c = getchar();
    switch(c){
      case 'q': // Shutdown/Quit - Delete Tasks in correct order
      case 'l': // Large block count 0
                semTake(CountSemID);
                printf("Total Large blocks counted: %c \n", LargeCount0);
                semGive(CountSemID);
                break;
      case 's': // Small block count 0
                semTake(CountSemID);
                printf("Total Small blocks detected: %c \n", SmallCount0);
                semGive(CountSemID);
                break;
      case 'o': // Large block count 1
                semTake(CountSemID);
                printf("Total Large blocks counted: %c \n", LargeCount1);
                semGive(CountSemID);
                break;
      case 'p': // Small block count 1
                semTake(CountSemID);
                printf("Total Small blocks detected: %c \n", SmallCount1);
                semGive(CountSemID);
                break;
      case 'k': // Reset count value for large blocks
                semTake(EndCountSemID);
                printf("Reset Large block count value to 0 \n");
                LargeCount = 0;
                LargeCountSensor0 = 0;
                LargeCountSensor1 = 0;
                semGive(CountSemID);
                break;
      case 'a': // Reset count value for small blocks
                semTake(CountSemID);
                printf("Reset Small block detected value to 0 \n");
                SmallCount = 0;
                semGive(CountSemID);
                break;
      case 'z': // Large block count Conveyor 0
                semTake(EndCountSemID);
                printf("Large block count for conveyor 0: %c \n", LargeCountSensor0);
                semGive(EndCountSemID);
                break;
      case 'x': // Large block count Conveyor 1
                semTake(EndCountSemID);
                printf("Large block count for conveyor 1: %c \n", LargeCountSensor1);
                semGive(EndCountSemID);
                break;
      default: break;
    }

  }
}
/////////////////////////// Motor Response //////////////////////////

void MotorController0(void){
  int res;
  while(1){
    // Wait for message from timer conveyor 0
    res = msgQRecieve(queueMotorC0ID, &State, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 0 message queue! Terminating...");
      exit(0);
    }
    // Check current gat state
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (State == 1){ // Down
      switch CheckGateState{
        case 0 : NextState = 1;
                 break;
        case 1 : NextState = 1;
                 break;
        case 2 : NextState = 3;
                 break;
        case 3 : NextState = 3;
                 break;
        default : NextState = 0;
                  break;
      }
    }
    else if (State == 0){ // Up
      switch CheckGateState{
        case 0 : NextState = 0;
                 break;
        case 1 : NextState = 0;
                 break;
        case 2 : NextState = 2;
                 break;
        case 3 : NextState = 2;
                 break;
        default : NextState = 0;
                  break;
      }
    }
  setGates(char NextState);
  GateState = NextState;
  semGive(MotorStateSemID);
  // Check timing on calling this function/Include a delay/only on UP gate (i.e. large block)
  CheckEndSensor0();
  }
}
void MotorController1(void){
  int res;
  while(1){
    // Wait for message from timer conveyor 1
    res = msgQRecieve(queueMotorC1ID, &State, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 0 message queue! Terminating...");
      exit(0);
    }
    // Check current gat state
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (State == 1){ // Down
      switch CheckGateState{
        case 0 : NextState = 2;
                 break;
        case 1 : NextState = 3;
                 break;
        case 2 : NextState = 2;
                 break;
        case 3 : NextState = 3;
                 break;
        default : NextState = 0;
                  break;
      }
    }
    else if (State == 0){ // Up
      switch CheckGateState{
        case 0 : NextState = 0;
                 break;
        case 1 : NextState = 1;
                 break;
        case 2 : NextState = 0;
                 break;
        case 3 : NextState = 1;
                 break;
        default : NextState = 0;
                  break;
    }
    setGates(char NextState);
    GateState = NextState;
    semGive(MotorStateSemID);
    // Check timing on calling this function/Include a delay/only on UP gate (i.e. large block)
    CheckEndSensor0();
  }
}

//////////////////////////// Timers /////////////////////////

void TimerT1Callback(State){ // Conveyor 0
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT2Callback(State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT3Callback(State){ // Conveyor 0
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT4Callback(State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC1ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}

void closeGateTimer0(void){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, 2*sysClkRateGet(), (FUNCPTR)TimerT1Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}

void closeGateTimer1(void){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, 2*sysClkRateGet(), (FUNCPTR)TimerT2Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}
void openGateTimer0(void){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, 2*sysClkRateGet(), (FUNCPTR)TimerT3Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}

void openGateTimer1(void){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, 2*sysClkRateGet(), (FUNCPTR)TimerT4Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}
///////////////////////// Block Analysis ////////////////////////////

void AnalyseConveyor(CurrentState, LastState, Conveyor){
int res;
if (Conveyor == 0){
    switch(CurrentState){
      case 0 : // Small block
               if (LastState == 1){
                 if (BlockTimeQueue0 != 0){
                   openGateTimer0(2);
                   closeGateTimer0(4);

                   semTake(CountSemID, WAIT_FOREVER);
                   SmallCount0 ++;
                   semGive(CountSemID, WAIT_FOREVER);
                 }
                 else{
                   clock_gettime(CLOCK_REALTIME, &time);

                   semTake(CountSemID, WAIT_FOREVER);
                   SmallCount0 ++;
                   semGive(CountSemID, WAIT_FOREVER);
                 }
               } // second small block
               else if (LastState == 3){
                 clock_gettime(CLOCK_REALTIME, &time);

                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount0 ++;
                 semGive(CountSemID, WAIT_FOREVER);
               }
               break;
      case 3 : if (LastState != 0){
                 // add count to large block
                 semTake(CountSemID, WAIT_FOREVER);
                 LargeCount0 ++;
                 semGive(CountSemID, WAIT_FOREVER);
               }
      default :
               break;
    }
}
else if (Conveyor == 1){
  switch(CurrentState){
    case 0 : // Small block
             if (LastState == 1){
               if (BlockTimeQueue1 != 0){
                 openGateTimer1(2);
                 closeGateTimer1(4);

                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount1 ++;
                 semGive(CountSemID, WAIT_FOREVER);
               }
               else{
                 clock_gettime(CLOCK_REALTIME, &time);


                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount1 ++;
                 semGive(CountSemID, WAIT_FOREVER);
               }
             } // second small block
             else if (LastState == 3){
               clock_gettime(CLOCK_REALTIME, &time);

               semTake(CountSemID, WAIT_FOREVER);
               SmallCount1 ++;
               semGive(CountSemID, WAIT_FOREVER);
             }
             break;
    case 3 : if (LastState != 0){
               // add count to large block
               semTake(CountSemID, WAIT_FOREVER);
               LargeCount1 ++;
               semGive(CountSemID, WAIT_FOREVER);
             }
    default :
             break;
  }
}
}

///////////////////////////// Sensor Input ///////////////////////////

void CheckSensor(){
  int res;
  char CurrentState0;
  char CurrentState1;
  char LastState0;
  char LastState1;
  while(1){
  // Reset sensor before use
    resetSizeSensors(0);
    char CurrentState0 = readSizeSensors(0);
    if (CurrentState0 != LastState0){
      AnalyseConveyor(CurrentState0, LastState0, 0);
    }
    LastState0 = CurrentState0;

    resetSizeSensors(1);
    char CurrentState1 = readSizeSensors(1);
    if (CurrentState1 != LastState1){
      AnalyseConveyor(CurrentState1, LastState1, 1);
    }
    LastState1 = CurrentState1;
  }
}

void CheckEndSensor0(void){
  resetCountSensor(0);
  char Count = readCountSensor(0);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor0 ++;
    semGive(EndCountSemID, WAIT_FOREVER);
    }
}
void CheckEndSensor1(void){
  resetCountSensor(1);
  char Count = readCountSensor(1);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor1 ++;
    semGive(EndCountSemID, WAIT_FOREVER);
  }
}
///////////////////////////// Setup ////////////////////////

void Main(void){
  startMotor();
  int CheckSensor_id;
  int MotorController0_id;
  int MotorController1_id;
  int Interface_id;
  // Set up message queues
  queueMotorC0ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueMotorC0ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  queueMotorC1ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueMotorC1ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  // Set up semaphores
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
  // Set up tasks
  CheckSensor_id = taskSpawn("CheckSensor", 100, 0, 20000,
                      (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  Interface_id = taskSpawn("Interface", 101, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  MotorController0_id = taskSpawn("MotorController0", 101, 0, 20000,
                      (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  MotorController1_id = taskSpawn("MotorController1", 101, 0, 20000,
                      (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  while(1){
    // Loops forever and let the tasks/functions sort themselves out.
    // Show debug prints here?
    // Should this be a while(1)??
  }
}

// readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
// sensor 1 & 2, respectively.

// TO BE DONE
// Shutdown Code
// Interface
