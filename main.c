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
    if(State[i] != LasteState[i]) {
      return i;
    }
  }
  // State has not changed
  return 0;
}

void CheckSensor(char State[], char LastState[]){
  // Check sensors and Count operation
  // Sensors will cycle through State 1, 2 if small block
  // Sensors will cycle through State 1, 3, 2 if large block

  // If reset is true, reset buffers
  if State[5] == 1{
    State[] = {0, 0, 0, 0, 0, 0};
    unsigned char LastState[] = {0, 0, 0, 0, 0, 0};
    unsigned char LargeBlockCount0 = 0;
    unsigned char SmallBlockCount0 = 0;
    unsigned char LargeBlockCount1 = 0;
    unsigned char SmallBlockCount1 = 0;
    unsigned char BlockBuffer0 = 0;
    unsigned char BlockBuffer1 = 0;
  }
  // readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
  // sensor 1 & 2, respectively.
  // The conveyor parameter distinguishes which conveyor is being checked.
  // Input is either 0 or 1.
  char BlockSize0 = readSizeSensors(0);
  char BlockSize1 = readSizeSensors(1);

  // Update block count if object is detected. Only perform this if an object is present.
  if (BlockSize0 != 0){
    LargeBlockCount0, SmallBlockCount0, BlockBuffer0 = BlockCount(char BlockSize0,
                                                                  char BlockBuffer0,
                                                                  char LargeBlockCount0,
                                                                  char SmallBlockCount0);
  }
  if (BlockSize1 !=0){
    LargeBlockCount1, SmallBlockCount1, BlockBuffer1 = BlockCount(char BlockSize1,
                                                                  char BlockBuffer1,
                                                                  char LargeBlockCount1,
                                                                  char SmallBlockCount1);
  }

  State[1] = LargeBlockCount0; // 0 == conveyor 1
  State[2] = SmallBlockCount0;
  State[3] = LargeBlockCount1; // 1 == conveyor 2
  State[4] = SmallBlockCount1;

  // Put data into an array and compare with previous State. Flip State change
  // to 0 if identical. Otehrwse State will define small/large block passing and
  // which conveyor.
  State[0] = CompareBuffers(char State[], char LastState[])
  // Save previous State.
  strncpy(char LastState, State, 6);
  // return array of 6 characters
  return State[], LastState[];
}

void MotorController(char State[], char LastState[]){
  // Gate controller operation controls both gates
  // Return number of Large Blocks sorted
  // Use count sensor to adjust when to close the gate?
  setGates(char INPUT);
}

void Feedback(LargeBlockDetectConveyor1, SmallBlockDetectConveyor1,
              LargeBlockCountConveyor1){
  // User interface that returns number of large blocks detected,
  // snall blocks detected and large blocks collected.
  // Includes error catching and shutdown option.
  // Shutdown option may need to be an interrupt to override operations.
}

void Main(void){
  // char State buffer holds conveyor info.
  // State change | Large block count conveyor 1 | Small block count conveyor 1 |
  // Large block count conveyor 2 | Small block count conveyor 2 | Reset
  // 0 is null or no State change
  // Initialise State buffer to 0
  unsigned char State[] = {0, 0, 0, 0, 0, 1};
  unsigned char LastState[] = {0, 0, 0, 0, 0, 0};
  startMotor(); // startMotor function from cinterface.h
  while(1){
    Settings();
    // Update current State
    State[], LastState[] = CheckSensor(char State[], char LastState[]);
    // If there is a State change then Motor is used.
    if (State[0] != 0){
      MotorController(char State[], LastState[]);
    }
    // Should Feedback be in conditional statement or not?
    Feedback();
  }
}
