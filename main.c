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
char BlockTimePointer0;
char BlockTimePointer1;

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
SEM_ID BlockTimeSemID;

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
                printf("Shutting down... \n");
                taskDelete(CheckSensor_id); // can you delete tasks outside of main?
                taskDelete(MotorController0_id);
                taskDelete(MotorController1_id);
                printf("Goodbye! \n");
                taskDelete(Interface_id); //  Can you delete a task you are in?
      case 'l': // Large block count 0
                printf("Total Large blocks counted: %c \n", LargeCount0);
                break;
      case 's': // Small block count 0
                printf("Total Small blocks detected: %c \n", SmallCount0);
                break;
      case 'o': // Large block count 1
                printf("Total Large blocks counted: %c \n", LargeCount1);
                break;
      case 'p': // Small block count 1
                printf("Total Small blocks detected: %c \n", SmallCount1);
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
                printf("Large block count for conveyor 0: %c \n", LargeCountSensor0);
                break;
      case 'x': // Large block count Conveyor 1
                printf("Large block count for conveyor 1: %c \n", LargeCountSensor1);
                break;
      case 'h': printf("Welcome to block conveyor 4.0! \n
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
              break;
      default: break;
    }

  }
}
/////////////////////////// Motor Response //////////////////////////

// Shifts states in the buffer along
void ShiftBuffer(char Conveyor){
  semTake(BlockTimeSemID, WAIT_FOREVER);
  if (Conveyor == 0){
    for (char i = 1; i < 4; i--) {
      BlockTimeQueue0[i] = BlockTimeQueue0[i-1]
    }
  }
  else if (Conveyor == 1){
    for (char i = 1; i < 4; i--) {
      BlockTimeQueue1[i] = BlockTimeQueue1[i-1]
    }
  }
  semGive(BlockTimeSemID, WAIT_FOREVER);
}

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
    if (State == DOWN){ // Down
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
      closeGateTimer0(2);
    }
    else if (State == UP){ // Up
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
  semTake(BlockTimeSemID, WAIT_FOREVER);
  if (BlockTimePointer0 == 1){ // Reset pointer
    BlockTimePointer0 = 0;
  }
  else if (BlockTimePointer0 > 1){ // If pointer greater than 1 then there is another block on the conveyor
    clock_gettime(CLOCK_REALTIME, &endtime);
    char time = &endtime - BlockTimeQueue0[1];
    closeGateTimer0(time); // Reset timer with remaining time
    ShiftBuffer(0);
    BlockTimePointer0 --;
  }
  semGive(BlockTimeSemID);
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
    if (State == DOWN){ // Down
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
      //openGateTimer1(2);
    }
    else if (State == UP){ // Up
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
    semTake(BlockTimeSemID, WAIT_FOREVER);
    if (BlockTimePointer1 == 1){
      BlockTimePointer1 = 0;
    }
    else if (BlockTimePointer1 > 1){
      clock_gettime(CLOCK_REALTIME, &endtime);
      char time = &endtime - BlockTimeQueue1[1];
      closeGateTimer1(time); // Reset timer
      ShiftBuffer(1);
      BlockTimePointer1 --;
    }
    semGive(BlockTimeSemID);
    // Check timing on calling this function/Include a delay/only on UP gate (i.e. large block)
    CheckEndSensor1();
  }
}

//////////////////////////// Timers /////////////////////////

void TimerT1Callback(har State){ // Conveyor 0
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT2Callback(char State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT3Callback(char State){ // Conveyor 0
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC0ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT4Callback(char State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC1ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}

void closeGateTimer0(char time){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, time*sysClkRateGet(), (FUNCPTR)TimerT1Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}

void closeGateTimer1(char time){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, time*sysClkRateGet(), (FUNCPTR)TimerT2Callback, DOWN);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}
void openGateTimer0(char time){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, time*sysClkRateGet(), (FUNCPTR)TimerT3Callback, UP);
  if (res == ERROR){
    printf("Cannot start the timer! Terminating...");
    exit(0);
    }
}

void openGateTimer1(char time){
	printf("Creating close gate timer");

	int res;
  res = wdStart(timer_T1_ID, time*sysClkRateGet(), (FUNCPTR)TimerT4Callback, UP);
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
                 if (BlockTimePointer0 != 0){
                   //openGateTimer0(4);
                   closeGateTimer0(2);
                   // Update count variables
                   semTake(CountSemID, WAIT_FOREVER);
                   SmallCount0 ++;
                   semGive(CountSemID);
                   // Update pointer for block queue
                   semTake(BlockTimeSemID, WAIT_FOREVER);
                   BlockTimePointer0 ++;
                   semGive(BlockTimeSemID);
                 }
                 else{
                   clock_gettime(CLOCK_REALTIME, &time);
                   // Update count variables
                   semTake(CountSemID, WAIT_FOREVER);
                   SmallCount0 ++;
                   semGive(CountSemID);
                   // Update pointer for bock queue
                   semTake(BlockTimeSemID, WAIT_FOREVER);
                   BlockTimeQueue0[BlockTimePointer0] = &time;
                   BlockTimePointer0 ++;
                   semGive(BlockTimeSemID);
                 }
               } // second small block
               else if (LastState == 3){
                 clock_gettime(CLOCK_REALTIME, &time);
                 // Update count variables
                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount0 ++;
                 semGive(CountSemID);
                 // Update block queue pointer
                 semTake(BlockTimeSemID, WAIT_FOREVER);
                 BlockTimeQueue0[BlockTimePointer0] = &time;
                 BlockTimePointer0 ++;
                 semGive(BlockTimeSemID);
               }
               break;
      case 3 : if (LastState != 0){
                 // add count to large block
                 semTake(CountSemID, WAIT_FOREVER);
                 LargeCount0 ++;
                 semGive(CountSemID);
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
                 //openGateTimer1(4);
                 closeGateTimer1(2);
                 // Update count variable
                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount1 ++;
                 semGive(CountSemID);
                 // Update block queue pointers
                 semTake(BlockTimeSemID, WAIT_FOREVER);
                 BlockTimePointer1 ++;
                 semGive(BlockTimeSemID);
               }
               else{
                 clock_gettime(CLOCK_REALTIME, &time);
                 // Update count variable
                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount1 ++;
                 semGive(CountSemID);
                 // Updte block queue pointer
                 semTake(BlockTimeSemID, WAIT_FOREVER);
                 BlockTimeQueue0[BlockTimePointer1] = &time;
                 BlockTimePointer1 ++;
                 semGive(BlockTimeSemID);
               }
             } // second small block
             else if (LastState == 3){
               clock_gettime(CLOCK_REALTIME, &time);
               // Update count variable
               semTake(CountSemID, WAIT_FOREVER);
               SmallCount1 ++;
               semGive(CountSemID);
               // Update block queue pointer
               semTake(BlockTimeSemID, WAIT_FOREVER);
               BlockTimeQueue0[BlockTimePointer1] = &time;
               BlockTimePointer1 ++;
               semGive(BlockTimeSemID);
             }
             break;
    case 3 : if (LastState != 0){
               // add count to large block
               semTake(CountSemID, WAIT_FOREVER);
               LargeCount1 ++;
               semGive(CountSemID);
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
  BlockTimeSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (BlockTimeSemID == NULL){
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
  printf("Welcome to block conveyor 4.0! \n
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

  while(1){
    // Loops forever and let the tasks/functions sort themselves out.
    // Show debug prints here?
    // Should this be a while(1)??
  }
}

// TO BE DONE
// Shutdown Code
