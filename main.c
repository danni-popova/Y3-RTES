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

// // Possible addition is user settings to change mode.
// void Settings(void){
//   // Initial settings that can be altered by the user
// }
// void Interface(void){}
//
void BlockCount(void){
  // Wait for message to be recieved
  // Save message to array
  // small == 1, 0 -- Use for counting
  // large == 1, 1 -- Use for counting
  // 1 small == 1, 0, 2
  // 1 large == 1, 1, 3
  // 2 small == 0, 3, 0
  // 2 large == 2, 3, 1
  // large then small == 2, 3, 0
  // small then large == 0, 3, 1
  // end 'bit' == 2, 0

  // If array contains correct message code, indicate for motor controller to
  // start
}

// How fast to poll the sensors?
void CheckSensor(){
  while(1){
    // Reset sensor before use
    // Sensors MUST be read SYNCHRONOUSLY (every 'tick')
    char BlockSize0 = readSizeSensors(0);
    char BlockSize1 = readSizeSensors(1);

    // Update Conveyor 1 send BlockSize0

    // Update Conveyor 2 send BlockSize1

  }
}

void MotorController(){
  // Gate controller operation controls both gates
  setGates(char INPUT);
}

// void Feedback(){
//   // User interface that returns number of large blocks detected,
//   // snall blocks detected and large blocks collected.
//   // Includes error catching and shutdown option.
//   // Shutdown option may need to be an interrupt to override operations.
// }

void Main(void){
// Tasks cannot return values, only pass arguments IN.
// Tasks will run the passive components of the code, the analysis will be
// done in main()
// Find how to pass information from one place to another - use message queues
// or flags

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
    // Sensors will cycle through State 1, 0, 2 if small block
    // Sensors will cycle through State 1, 3, 2 if large block
    // Sensors will cycle through State 1, 0, 3 if 2 small blocks
    // readSizeSensors returns 0, 1, 2, 3 for no object, sensor 1, sensor 2, and
    // sensor 1 & 2, respectively.
    // The conveyor parameter distinguishes which conveyor is being checked.
    // Input is either 0 or 1.
    // CheckSensor may not be possible to do as a Task
    // How to set a flag without using global variables?
    CheckSensor_id = taskSpawn("CheckSensor", 100, 0, 20000,
                        (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

    // Block count does not need to be a task!
    //BlockCount();

    // Task that controls the gates for a given input
    MotorController_id = taskSpawn("MotorController", 100, 0, 20000,
                        (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);

    // Same as interface? Is this needed?
    //Feedback();
  }
}
