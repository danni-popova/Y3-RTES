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

void CheckSensor(char CountConveyor1, char ObjectSizeConveyor1){
  // Check sensors and Count operation
  // Sensors will cycle through state 0, 1, 2 if small block
  // Sensors will cycle through state 0, 1, 3, 2 if large block
  readSizeSensors(char conveyor);
}

void MotorController(void){
  // Gate controller operation controls both gates
  setGates(char state);
}

void Feedback(LargeBlockDetectConveyor1, SmallBlockDetectConveyor1,
              LargeBlockCountConveyor1){
  // User interface that returns number of large blocks detected,
  // snall blocks detected and large blocks collected.
  // Includes error catching and shutdown option.
  // Shutdown option may need to be an interrupt to override operations.
}

void Main(void){
  startMotor(); // startMotor function from cinterface.h
  while(1){
    Settings();
    CheckSensor();
    MotorController();
    Feedback();
  }
}
