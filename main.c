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

char CompareBuffers(char State[], char LastState[]){
  for(char i=1; i<5; i++) {     // Iterate through State buffers from 1 to 5
    if(State[i] != LasteState[i]) {
      return 1;
    }
     // State has not changed
     return 0;
  }
}

void CheckSensor(char State[], char LastState[]){
  // Check sensors and Count operation
  // Sensors will cycle through State 0, 1, 2 if small block
  // Sensors will cycle through State 0, 1, 3, 2 if large block

  // If reset is true, reset buffers
  if State[5] == 1{
    State[] = {0, 0, 0, 0, 0, 0};
    unsigned char LastState[] = {0, 0, 0, 0, 0, 0, 0};
  }
  // readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
  // sensor 1 & 2, respectively.
  // The conveyor parameter distinguishes which conveyor is being checked.
  // Input is either 0 or 1.
  char State[1] = readSizeSensors(0);
  char State[2] = readSizeSensors(1);
  // Put data into an array and compare with previous State. Flip State change
  // to 0 if identical.
  State[0] = CompareBuffers(char State[], char LastState[])
  // Save previous State.
  strncpy(char LastState, State, 6);
  // return array of 6 characters
}

void MotorController(void){
  // Gate controller operation controls both gates
  setGates(char State);
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
  // State change | State conveyor 1 | State conveyor 2 |
  // count conveyor 1 | count conveyor 2 | initialise or reset
  // 0 is null or no State change
  // Initialise State buffer to 0
  unsigned char State[] = {0, 0, 0, 0, 0, 1};
  unsigned char LastState[] = {0, 0, 0, 0, 0, 0};
  startMotor(); // startMotor function from cinterface.h
  while(1){
    Settings();
    // Update current State
    State = CheckSensor(State[], LastState[]);
    // If there is a State change then Motor is used.
    if (State[0] == 1){
      MotorController();
    }
    // Should Feedback be in conditional statement or not?
    Feedback();
  }
}
