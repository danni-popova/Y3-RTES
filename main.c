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

char S[2] = {1, 0, 2}; // Do not count, Influence gates
char L[2] = {1, 3, 2}; // Only count, do not influence gates
char SS[2] = {0, 3, 0}; // Do not count, Influence gates
char LL[2] = {2, 3, 1}; // Do not count, do not influence gates, Do we need this?
char SL[2] = {0, 3, 1}; // Count AND influence gates
char LS[2] = {2, 3, 0}; // Count AND influence gates
char Reset[2] = {0, 0, 0};
char GateState;
char C0StartFlag;
char C1StartFlag;

// // Possible addition is user settings to change mode.
// void Settings(void){
//   // Initial settings that can be altered by the user
// }
// void Interface(void){
// // Shutdown
//}
//

char identical(char a[], char b[]) {
    for (char i = 0 ; i < 3 ; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

// This is a function that the conveyors will need to SHARE
void AnalyseBlocks(void){
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
  char Logic[5]; {S, L, SS, LL, SL, LS};

  // If array contains correct message code, send message of what the MotorController
  // needs to do and when
  // Send count msg to Interface
  while(1){
    State = 0;
    for (char i = 0; i < 5; i++){
      State = identical(Logic[i], BlockBuffer);
      if (State == 1){
        // Pass message to Gates
        return Logic[i]; // If i == 6 then Reset flag
        break;
      }
    }
  }
}

void AnalyseConveyor0(){
  char BlockBuffer0[2] = {0};
  while(1){
    // If a sensor detects 1 for the first time, set the start flag
    if (BlockSize0 == 1 && C0StartFlag == 0){
      C0StartFlag = 1;
    }
    // Check Flag and that the next state isn't a duplicate
    if (C0StartFlag == 1 && BlockSize0 != BlockBuffer0[0]){
      BlockBuffer0 = BufferFunction(BlockBuffer0, BlockSize0)
      // Send buffer somewhere
    }
  }
}

void AnalyseConveyor1(){
  char BlockBuffer1[2] = {0};
  while(1){
    // If a sensor detects 1 for the first time, set the start flag
    if (BlockSize1 == 1 && C1StartFlag == 0){
      C1StartFlag = 1;
    }
    if (C1StartFlag == 1 && BlockSize0 != BlockBuffer0[0]){
      BlockBuffer1 = BufferFunction(BlockBuffer1, BlockSize1)
      // Send buffer somewhere
    }
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

// How fast to poll the sensors?
// This might be a function
void CheckSensor(){
  while(1){
    // Reset sensor before use
    // Sensors MUST be read SYNCHRONOUSLY (every 'tick')
    char BlockSize0 = readSizeSensors(0);
    // Send to analyses
    char BlockSize1 = readSizeSensors(1);
    // Send to analyses

  }
}
// Sort timing for BOTH conveyors!!
void MotorController(char GateState){
  // Gate controller operation controls both gates
  // Recieve msg from Buffer and control gates correspondingly
  // Gates need to stay open for a certain amount of time so this should be
  // amended accordingly
  // Recieve DOWN or UP or BOTH from buffers, compare buffers for both belts
  setGates(char GateState);
  // Send message to count sensor to read the count sensor at correct time
}

// void Feedback(){
//   // User interface that returns number of large blocks detected,
//   // snall blocks detected and large blocks collected.
//   // Includes error catching and shutdown option.
//   // Shutdown option may need to be an interrupt to override operations.
// }

void Main(void){
  startMotor();
  while(1){
    int CheckSensor_id;
    int MotorController_id;

    // Setup that can be configured via the interface (include later)
    //Settings();

    // Poll for a keyboard input and respond accordinly
    // This can be a Task
    // Message queue for keyboard inputs
    // Interface();

    // Check sensors and Count operation
    // readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
    // sensor 1 & 2, respectively.
    // The conveyor parameter distinguishes which conveyor is being checked.
    // Input is either 0 or 1.
    CheckSensor_id = taskSpawn("CheckSensor", 100, 0, 20000,
                        (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

    // Does this need to be a task?
    MotorController_id = taskSpawn("MotorController", 100, 0, 20000,
                        (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);
  }
}
