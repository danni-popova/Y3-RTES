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

// Possible addition is user settings to change mode.
void Settings(void){
  // Initial settings that can be altered by the user
}
void Interface(void){}

void BlockCount(char BlockSize, char BlockBuffer, char LargeBlockCount,
                char SmallBlockCount){
  if (BlockSize == 1){
    BlockBuffer = 1;
    }
  if (BlockBuffer == 1){
    switch (BlockSize){
      case 0: printf("Error: Block has been removed from belt");
      // Reset?
      case 1: printf("Error: Conveyor belt motor has stopped");
      case 2: SmallBlockCount = SmallBlockCount + 1;
      case 3: LargeBlockCount = LargeBlockCount + 1;
      default: printf("Error: Block size unknown!");
    }
    BlockBuffer = 0;
  }
  return LargeBlockCount, SmallBlockCount, BlockBuffer;
}

char CompareBuffers(char State[], char LastState[]){
  for(char i=1; i<4; i++) {     // Iterate through State buffers from 1 to 4
    if(State[i] != LasteState[i]){

    }
  }
  return 0;
}

void CheckSensor(){

  // Reset sensor before use
  char BlockSize0 = readSizeSensors(0);
  char BlockSize1 = readSizeSensors(1);
}

void MotorController(){
  // Gate controller operation controls both gates
  // Return number of Large Blocks sorted
  setGates(char INPUT);
}

void Feedback(){
  // User interface that returns number of large blocks detected,
  // snall blocks detected and large blocks collected.
  // Includes error catching and shutdown option.
  // Shutdown option may need to be an interrupt to override operations.
}

void Main(void){
// Tasks cannot return values, only pass arguments IN.
// Tasks will run the passive components of the code, the analysis will be
// done in main()

  startMotor();
  while(1){

    // Setup that can be configured via the interface
    Settings();

    // Poll for a keyboard input and respond accordinly
    Interface();

    // Check sensors and Count operation
    // Sensors will cycle through State 1, 2 if small block
    // Sensors will cycle through State 1, 3, 2 if large block
    // readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
    // sensor 1 & 2, respectively.
    // The conveyor parameter distinguishes which conveyor is being checked.
    // Input is either 0 or 1.
    CheckSensor();

    // Block count does not need to be a task!
    BlockCount();

    // Task that controls the gates for a given input
    MotorController();

    // Same as interface? Is this needed?
    Feedback();
  }
}
