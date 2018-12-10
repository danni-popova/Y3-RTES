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

char S[2] = {1, 0, 2}; // Do not count, Influence gates // State 0
char L[2] = {1, 3, 2}; // Only count, do not influence gates // State 1
char SS[2] = {0, 3, 0}; // Do not count, Influence gates // State 2
char LL[2] = {2, 3, 1}; // Do not count, do not influence gates, Do we need this? // State 3
char SL[2] = {0, 3, 1}; // Count AND influence gates // State 4
char LS[2] = {2, 3, 0}; // Count AND influence gates // State 5
char ResetState[2] = {0, 0, 0}; // State 6
char ResetFlag;
char GateState; // Check and Set GateState before execution (after timer)
char C0StartFlag;
char C1StartFlag;
WD_ID timer_T1_ID; // Opening timer for conveyor 0
WD_ID timer_T2_ID; // Opening timer for conveyor 1
WD_ID timer_T3_ID; // Closing timer for conveyor 0
WD_ID timer_T4_ID; // Closing timer for conveyor 1
MSG_Q_ID queue0ID;
MSG_Q_ID queue1ID;
SEM_ID AnalyseBlocksSemID;
SEM_ID MotorStateSemID;

void TimerT1Callback(void){ // Conveyor 0 close gate timer
  // Wait for semaphore
  semTake(MotorStateSemID, WAIT_FOREVER);
  // Check gate state
  // Alter gate state
  semGive(MotorStateSemID)
  // Send message to MotorController
}
void TimerT2Callback(void){ // Conveyor 1 close gate timer
  // Wait for semaphore
  semTake(MotorStateSemID, WAIT_FOREVER);
  // Check gate state
  // Alter gate state
  semGive(MotorStateSemID)
  // Send message to MotorController
}
void TimerT3Callback(void){ // Conveyor 0 open gate timer
  semTake(MotorStateSemID, WAIT_FOREVER);
  // Check gate state
  // Alter gate state
  semGive(MotorStateSemID)
}
void TimerT4Callback(void){ // Conveyor 1 open gate timer
  semTake(MotorStateSemID, WAIT_FOREVER);
  // Check gate state
  // Alter gate state
  semGive(MotorStateSemID)
}

// // Possible addition is user settings to change mode.
// void Settings(void){
//   // Initial settings that can be altered by the user
// }

void Interface(void){
  // Shutdown
  // Restart
  // Poll for a keyboard input and respond accordingly
}

// Check whether the state has changed
char identical(char a[], char b[]) {
    for (char i = 0 ; i < 3 ; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

void AnalyseBlocks(char BlockBuffer[]){
  char Logic[5] = {S, L, SS, LL, SL, LS};
  // If array contains correct message code, send message of what the MotorController
  // needs to do and when
  while(1){
    State = 0;
    if (ResetFlag == 1){
      return 6; // If i == 6 then Reset flag
    }
    for (char i = 0; i < 5; i++){
      State = identical(Logic[i], BlockBuffer);
      if (State == 1){
        // Pass message to Gates
        return i; // If i == 6 then Reset flag NEED TO IMPLEMENT
        break;
      }
    }
    return 7;
  }
}

// Shifts states in the buffer along
void BufferFunction(char BlockBuffer[], char BlockSize){
    for (char i = 1; i < 3; i--) {
      BlockBuffer[i] = BlockBuffer[i-1]
    }
    BlockBuffer[0] = BlockSize;
    return BlockBuffer;
}

void AnalyseConveyor0(){
  char BlockBuffer0[2] = {0};
  char State;
  int res;
  timer_T1_ID = wdCreate();
  if (timer_T1_ID == NULL){
    printf("Cannot create timer! Terminating this task...");
    exit(0);
  }
  while(1){
    res = msgQRecieve(queue0ID, &BlockSize0, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 0 message queue! Terminating...");
      exit(0);
    }
    // If a sensor detects 1 for the first time, set the start flag
    if (BlockSize0 == 1 && C0StartFlag == 0){
      C0StartFlag = 1;
    }
    // Check Flag and that the next state isn't a duplicate
    if (C0StartFlag == 1 && BlockSize0 != BlockBuffer0[0]){
      semTake(AnalyseBlocksSemID, WAIT_FOREVER);
      BlockBuffer0 = BufferFunction(BlockBuffer0, BlockSize0)
      State = AnalyseBlocks(BlockBuffer0);
      semGive(AnalyseBlocksSemID);
      int timeinseconds;

      // Set timer depending upon buffer
      if (State < 7){
        switch(State){
          case 0 : timeinseconds = /*TIME1*/; // S
                   res = wdStart(timer_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT1Callback, State);
                   if (res == ERROR){
                     printf("Cannot start the timer! Terminating...");
                     exit(0);
                   }
                   C0StartFlag == 0;
                   break;
          case 1 : timeinseconds = /*TIME1*/; // L
                    res = wdStart(timer_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C0StartFlag == 0;
                   break;
          case 2 : timeinseconds = /*TIME2*/; // SS
                    res = wdStart(time_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C0StartFlag == 0;
                   break;
          case 3 : timeinseconds = /*TIME3*/; // LL
                    res = wdStart(time_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C0StartFlag == 0;
                   break;
          case 4 : timeinseconds = /*TIME2*/; // SL
                    res = wdStart(time_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C0StartFlag == 0;
                   break;
          case 5 : timeinseconds = /*TIME3*/; // LS
                    res = wdStart(time_T1_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C0StartFlag == 0;
                   break;
          case 6 : BlockBuffer0[2] = {0}; // Reset
                   C0StartFlag == 0;
                   break;
          default : printf("ERROR: Unknown State in state machine!!");
                    break;
        }
      }
    }
  }
}

void AnalyseConveyor1(){
  char BlockBuffer1[2] = {0};
  char State;
  int res;
  timer_T2_ID = wdCreate();
  if (timer_T2_ID == NULL){
    printf("Cannot create timer! Terminating this task...");
    exit(0);
  }
  while(1){
    // Wait for message
    res = msgQRecieve(queue1ID, &BlockSize1, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 1 message queue! Terminating...");
      exit(0);
    }
    // If a sensor detects 1 for the first time, set the start flag
    if (BlockSize1 == 1 && C1StartFlag == 0){
      C1StartFlag = 1;
    }
    // Run if StartFlag is set and the sensor reading is not the same
    // as the previous sensor reading
    if (C1StartFlag == 1 && BlockSize1 != BlockBuffer1[0]){
      semTake(AnalyseBlocksSemID, WAIT_FOREVER);
      BlockBuffer1 = BufferFunction(BlockBuffer1, BlockSize1)
      State = AnalyseBlocks(BlockBuffer1);
      semGive(AnalyseBlocksSemID);

      // Set timer depending upon buffer
      int timeinseconds;
      if (State < 7){ // or 6
        switch(State){
          case 0 : timeinseconds = /*TIME1*/; // S
                   res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                   if (res == ERROR){
                     printf("Cannot start the timer! Terminating...");
                     exit(0);
                   }
                   C1StartFlag == 0;
                   break;
          case 1 : timeinseconds = /*TIME1*/; // L
                    res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C1StartFlag == 0;
                   break;
          case 2 : timeinseconds = /*TIME2*/; // SS
                    res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C1StartFlag == 0;
                   break;
          case 3 : timeinseconds = /*TIME3*/; // LL
                    res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C1StartFlag == 0;
                   break;
          case 4 : timeinseconds = /*TIME2*/; // SL
                    res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C1StartFlag == 0;
                   break;
          case 5 : timeinseconds = /*TIME3*/; // LS
                    res = wdStart(timer_T2_ID, timeinseconds*sysClkRateGet(), (FUNCPTR)TimerT2Callback, State);
                    if (res == ERROR){
                      printf("Cannot start the timer! Terminating...");
                      exit(0);
                    }
                    C1StartFlag == 0;
                   break;
          case 6 : BlockBuffer1[2] = {0}; // Reset
                   C1StartFlag == 0;
                   break;
          case 7 : // No State change
                   break;
          default : printf("ERROR: Unknown State in state machine!!");
                    break;
        }
      }
    }
  }
}

void CheckSensor(){
  while(1){
  // Reset sensor before use
    char BlockSize0 = readSizeSensors(0);
    res = msgQSend(queue0ID, &BlockSize0, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
    if (res == ERROR){
      printf("Cannot send sensor 0 input into queue! Terminating...");
      exit(0);
    }

    char BlockSize1 = readSizeSensors(1);
    res = msgQSend(queue1ID, &BlockSize1, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
    if (res == ERROR){
      printf("Cannot send sensor 1 input into queue! Terminating...");
      exit(0);
    }
  }
}

void MotorController(char GateState){
// Gates need to stay open for a certain amount of time so this should be
// amended accordingly
  setGates(char GateState);
  // Set timer to read the count sensor at correct time
  // Check GateState before closing gate again
}

void Feedback(){
  // Read count sensor and store info for Interface to read
  // User interface that returns number of large blocks detected,
  // snall blocks detected and large blocks collected.
  // Includes error catching and shutdown option.
  // Shutdown option may need to be an interrupt to override operations.
}

void Main(void){
  startMotor();
  int CheckSensor_id;
  int MotorController_id;
  int AnalyseConveyor0_id;
  int AnalyseConveyor1_id;
  // Set up message queues
  queue0ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queue0ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  queue1ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queue1ID == NULL){
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
  // Set up tasks
  CheckSensor_id = taskSpawn("CheckSensor", 100, 0, 20000,
                      (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  AnalyseConveyor0_id = taskSpawn("AnalyseConveyor0", 101, 0, 20000,
                      (FUNCPTR)AnalyseConveyor0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  AnalyseConveyor1_id = taskSpawn("AnalyseConveyor1", 101, 0, 20000,
                      (FUNCPTR)AnalyseConveyor1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);
  // Does this need to be a task?
  //MotorController_id = taskSpawn("MotorController", 99, 0, 20000,
  //                    (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

  while(1){
    // Loops forever and let the tasks/functions sort themselves out.
    // Show debug prints here?
    // Should this be a while(1)??
  }
}

// N.B.
// small (1) == 1, 0 -- Use for counting (?) -- Send DOWN to MotorController with Default timing
// small (2) == 3, 0 -- Use for counting (?) -- Send DOWN with Default timing
// large == 1, 3 -- Use for counting (?) -- Send UP with Default timing
// 1 small == 1, 0, 2 -- Count?
// 1 large == 1, 1, 3 OR 1, 3, 2 -- Count?
// 2 small == 0, 3, 0 -- Send DOWN with Long timing (?)
// 2 large == 2, 3, 1 --  Send UP with Long timing (?)
// large then small == 2, 3, 0 -- Send UPDOWN
// small then large == 0, 3, 1 -- Send DOWNUP
// end 'bit' == 2, 0

// readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
// sensor 1 & 2, respectively.

// TO BE DONE
// Counter check needs to be implemented
// timers need times!
// gate close/open states need to be worked out and implemented
// Send up/down state to timer in switch statements
// Shutdown Code
// Interface
// counter sensor to be included!
