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

char S[2] = {1, 0, 2}; // State 0 Small
char L[2] = {1, 3, 2}; // State 1 Large
char SS[2] = {0, 3, 0}; // State 2 Small-Small
char LL[2] = {2, 3, 1}; // State 3 Large-Large
char SL[2] = {0, 3, 1}; // State 4 Small-Large
char LS[2] = {2, 3, 0}; // State 5 Large-Small
char ResetState[2] = {0, 0, 0}; // State 6
char ResetFlag;
char GateState; // Check and Set GateState before execution (after timer)
char C0StartFlag;
char C1StartFlag;
char SmallCount;
char LargeCount;
char LargeCountSensor0;
char LargeCountSensor1;
WD_ID timer_T1_ID; // Opening timer for conveyor 0
WD_ID timer_T2_ID; // Opening timer for conveyor 1
WD_ID timer_T3_ID; // Closing timer for conveyor 0
WD_ID timer_T4_ID; // Closing timer for conveyor 1
MSG_Q_ID queueSensorC0ID;
MSG_Q_ID queueSensorC1ID;
MSG_Q_ID queueMotorC0ID;
MSG_Q_ID queueMotorC1ID;
MSG_Q_ID queueCount;
SEM_ID AnalyseBlocksSemID;
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
      case 'l': // Large block count
                semTake(CountSemID);
                printf("Total Large blocks counted: %c \n", LargeCount);
                semGive(CountSemID);
      case 's': // Small block count
                semTake(CountSemID);
                printf("Total Small blocks detected: %c \n", SmallCount);
                semGive(CountSemID);
      case 'k': // Reset count value for large blocks
                semTake(CountSemID);
                printf("Reset Large block count value to 0 \n");
                LargeCount = 0;
                semGive(CountSemID);
      case 'a': // Reset count value for small blocks
                semTake(CountSemID);
                printf("Reset Small block detected value to 0 \n");
                SmallCount = 0;
                semGive(CountSemID);
      case 'z': // Large block count Conveyor 0
                semTake(EndCountSemID);
                printf("Large block count for conveyor 0: %c \n", LargeCountSensor0);
                semGive(EndCountSemID);
      case 'x': // Large block count Conveyor 1
                semTake(EndCountSemID);
                printf("Large block count for conveyor 1: %c \n", LargeCountSensor1);
                semGive(EndCountSemID);
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
void TimerT2Callback(State){ // Conveyor 0
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
void TimerT5Callback(State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC1ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}
void TimerT6Callback(State){ // Conveyor 1
  int res;
  // Send message to MotorController
  char res = msgQSend(queueMotorC1ID, &State, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
  if (res == ERROR){
    printf("Cannot send sensor 1 input into queue! Terminating...");
    exit(0);
  }
}

///////////////////////// Block Analysis ////////////////////////////

void AnalyseConveyor0(){
  char Flag = 0;
  char BlockSize0;
  int res;
  while(1){
    // Wait for message
    res = msgQRecieve(queueSensorC1ID, &BlockSize0, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 1 message queue! Terminating...");
      exit(0);
    }
    switch(BlockSize0){
      case 0 : // Small block
               // Check Flag
               // Clear Flag
               if (Flag == 1){
                 // Set timer and pass UP
                 res = wdStart(timer_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, UP);
                 if (res == ERROR){
                   printf("Cannot start the timer! Terminating...");
                   exit(0);
                   }
               }
               Flag = 0;
               break;
      case 1 : // Set flag
               Flag = 1;
               break;
      case 2 : // ??
      case 3 : // Large block, add to large block counter for counter check
               // Check flag, if not set, then not large block
               // Set gates DONW? Check counter sensor?
               // Clear Flag?
               break;
      default : // ??
               break;
    }
  }
}

void AnalyseConveyor1(){
  char Flag = 0;
  char BlockSize1;
  int res;
  while(1){
    // Wait for message
    res = msgQRecieve(queueSensorC1ID, &BlockSize1, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 1 message queue! Terminating...");
      exit(0);
    }
    switch(BlockSize1){
      case 0 : // Small block
               // Check Flag
               // Clear Flag
               if (Flag == 1){
                 // Set timer and pass UP
                 res = wdStart(timer_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, UP);
                 if (res == ERROR){
                   printf("Cannot start the timer! Terminating...");
                   exit(0);
                   }
               }
               Flag = 0;
               break;
      case 1 : // Set flag
               Flag = 1;
               break;
      case 2 : // ??
      case 3 : // Large block, add to large block counter for counter check
               // Check flag, if not set, then not large block
               // Set gates DONW? Check count sensor?
               // Clear Flag?
               break;
      default : // ??
               break;
    }
  }
}

///////////////////////////// Sensor Input ///////////////////////////

void CheckSensor(){
  int res;
  while(1){
  // Reset sensor before use
    resetSizeSensors(0);
    char BlockSize0 = readSizeSensors(0);
    res = msgQSend(queueSensorC0ID, &BlockSize0, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
    if (res == ERROR){
      printf("Cannot send sensor 0 input into queue! Terminating...");
      exit(0);
    }
    resetSizeSensors(1);
    char BlockSize1 = readSizeSensors(1);
    res = msgQSend(queueSensorC1ID, &BlockSize1, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
    if (res == ERROR){
      printf("Cannot send sensor 1 input into queue! Terminating...");
      exit(0);
    }
  }
}

void CheckEndSensor0(void){
  resetCountSensor(0);
  char Count = readCountSensor(0);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor0 = CountSensor + Count;
    semGive(EndCountSemID, WAIT_FOREVER);
    }
}
void CheckEndSensor1(void){
  resetCountSensor(1);
  Count = readCountSensor(1);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor1 = CountSensor + Count;
    semGive(EndCountSemID, WAIT_FOREVER);
  }
}
///////////////////////////// Setup ////////////////////////

void Main(void){
  startMotor();
  int CheckSensor_id;
  int MotorController_id;
  int AnalyseConveyor0_id;
  int AnalyseConveyor1_id;
  int Interface_id;
  // Set up message queues
  queueSensorC0ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueSensorC0ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  queueSensorC1ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueSensorC1ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
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
  queueCount = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueCount == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  // Set up semaphores
  AnalyseBlocksSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (AnalyseBlocksSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
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

  AnalyseConveyor0_id = taskSpawn("AnalyseConveyor0", 101, 0, 20000,
                      (FUNCPTR)AnalyseConveyor0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  AnalyseConveyor1_id = taskSpawn("AnalyseConveyor1", 101, 0, 20000,
                      (FUNCPTR)AnalyseConveyor1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  Interface_id = taskSpawn("Interface", 101, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  MotorController_id = taskSpawn("MotorController0", 101, 0, 20000,
                      (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  MotorController_id = taskSpawn("MotorController1", 101, 0, 20000,
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
// Counter check needs to be implemented
// timers need times!
// Shutdown Code
// Interface
// Check timing on count sensor function - only on large block?
// CHANGE ALL BLOCK LOGIC TO BE SIMPLER
// Add previous Check
// If encounter 0 then small block - set gates
// Don't count large blocks?
