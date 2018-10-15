// Catherine Flory && Danni Popova
// Real Time Embedded Systems

#include <cinterface.h>
#include <stdio.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <stdlib.h>

// Possible addition is user settings to change mode.

void Sensor1(void){

}

void Sensor2(void){

}

void MotorController(void){

}

void Feedback(LargeBlockDetect, SmallBlockDetect, LargeBlockCount){
  // User interface that returns number of large blocks detected,
  // snall blocks detected and large blocks collected.
  // Includes error catching and shutdown option.
  // Shutdown option may need to be an interrupt to override operations.
}

void Settings(void){
  // Initial settings that can be altered by the user
}

void Main(void){

}
