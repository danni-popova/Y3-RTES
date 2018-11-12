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
// void Interface(void){
// // Shutdown
//}
//
void AnalyseConveyor0(void){
  // Wait for message to be recieved
  // Check if msg is different to most recent buffer item
  // If so, Save message to array
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

  // If array contains correct message code, send message of what the MotorController
  // needs to do and when
  // Send count msg to Interface
  char BlockBuffer0[2] = {0};
  while(1){
    // RECEIVE MESSAGE
    for (char i = 1; i < 3; i--) {
      BlockBuffer0[i] = BlockBuffer0[i-1]
    }
    BlockBuffer0[0] = //MESSAGE
  }
}

void AnalyseConveyor1(void){

}

// How fast to poll the sensors?
void CheckSensor(){
  while(1){
    // Reset sensor before use
    // Sensors MUST be read SYNCHRONOUSLY (every 'tick')
    char BlockSize0 = readSizeSensors(0);
    char BlockSize1 = readSizeSensors(1);

    // SEMGIVE

    // Update Conveyor 1 send msg AnalyseConveyor0

    // Update Conveyor 2 send msg AnalyseConveyor1

  }
}
// Sort timing for BOTH conveyors!!
void MotorController(){
  // Gate controller operation controls both gates
  // Recieve msg from Buffer and control gates correspondingly
  // Gates need to stay open for a certain amount of time so this should be
  // amended accordingly
  // Recieve DOWN or UP or BOTH from buffers, compare buffers for both belts
  setGates(char INPUT);
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

    // Task that controls the gates for a given input
    MotorController_id = taskSpawn("MotorController", 100, 0, 20000,
                        (FUNCPTR)MotorController, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,);
  }
}
