#include "vxWorks.h"
#include "taskLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "cinterface.h"

int mainBeltPattern[] = {0, 0, 0};
int secondaryBeltPattern[] = {0, 0, 0};

int blocksDetected = 0;
int blocksCounted = 0;

int smallBlocksCounter = 0;
int bigBlocksCounter = 0;
char lastSensorResult = 0;

void main(void)
{
    startMotor();

    while(1)
    {
        
    }
}

void readMainSensors(void)
{
      // Reset has to be called before read
      resetSizeSensors(0);

      char result = readSizeSensors(0);

      if(result != lastSensorResult)
      {
          switch (result)
          {
            case 0:
              printf("No object currently in front of size sensors");
            case 1:
              printf("An object in front of first sensor");
              blocksDetected++;
            case 2:
              printf("An object in front of the second sensor");
            case 3:
              printf("Object(s) in front of both sensors");
          }
          // detect pattern change for small and big blocks
          if(lastSensorResult[0] == 1 && lastSensorResult[1]==0)
          {
              if(result == 0)
              {
                printf("Small block on belt 0");
                smallBlocksCounter++;
              }
              else if (result == 3)
              {
                printf("Big block on belt 0");
                bigBlocksCounter++;
              }
          }
      }
}

void readSecondarySensors(void)
{
    // Reset has to be called before read
    resetSizeSensors(1);

    char result = readSizeSensors(1);
}

void controlGates(char command)
{
    switch(command)
    {
      case 0 :
        printf("Opening both gates.");
        break;
      case 1 :
        printf("Open gate 0, close gate 1");
        break;
      case 2:
        printf("Close gate 0, open gate 1");
        break;
      case 3:
        printf("Closing both gates.");
        break;
    }

    setGates(command);
}

void callStopMotor(void)
{
    stopMotor();
}
