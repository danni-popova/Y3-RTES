#include "cinterface.h"
#include "stdio.h"
#include "vxWorks.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "stdlib.h"
#include "ioLib.h"
#include "wdLib.h"
#include "time.h"

char BlockTimePointer0;
char BlockTimePointer1;

char GateState;
char SmallCount0;
char SmallCount1;
char LargeCount0;
char LargeCount1;
char LargeCountSensor0;
char LargeCountSensor1;
WDOG_ID timer_T1_ID;
WDOG_ID timer_T2_ID;
WDOG_ID timer_T3_ID;
WDOG_ID timer_T4_ID;
WDOG_ID timer_T5_ID;
WDOG_ID timer_T6_ID;
MSG_Q_ID queueMotorC0ID;
MSG_Q_ID queueMotorC1ID;
SEM_ID MotorStateSemID;
SEM_ID CountSemID;
SEM_ID EndCountSemID;
SEM_ID BlockTimeSemID;
SEM_ID Timer0SemID;
SEM_ID Timer1SemID;


void Interface(void){
  char c;
  while(1){
    c = getchar();
    switch(c){
      /* case 'q':
                printf("Shutting down... \n");
                taskDelete(CheckSensor_id);
                taskDelete(MotorController0_id);
                taskDelete(MotorController1_id);
                printf("Goodbye! \n");
                taskDelete(Interface_id); */
      case 'l':
                printf("Total Large blocks counted: %c \n", LargeCount0);
                break;
      case 's':
                printf("Total Small blocks detected: %c \n", SmallCount0);
                break;
      case 'o':
                printf("Total Large blocks counted: %c \n", LargeCount1);
                break;
      case 'p':
                printf("Total Small blocks detected: %c \n", SmallCount1);
                break;
      case 'k':
                semTake(EndCountSemID, WAIT_FOREVER);
                printf("Reset Large block count value to 0 \n");
                LargeCount0 = 0;
                LargeCount1 = 0;
                LargeCountSensor0 = 0;
                LargeCountSensor1 = 0;
                semGive(CountSemID);
                break;
      case 'a':
                semTake(CountSemID, WAIT_FOREVER);
                printf("Reset Small block detected value to 0 \n");
                SmallCount0 = 0;
                SmallCount1 = 0;
                semGive(CountSemID);
                break;
      case 'z':
                printf("Large block count for conveyor 0: %c \n", LargeCountSensor0);
                break;
      case 'x':
                printf("Large block count for conveyor 1: %c \n", LargeCountSensor1);
                break;
/*      case 'h': printf("Welcome to block conveyor 4.0! \n
                Press: \n
                q to quit \n
                l to show number of large blocks counted \n
                s to show number of small blocks detected \n
                o to show number of small blocks counted \n
                p to show number of small blocks detected \n
                k to reset the large block counter \n
                a to reset the small block counter \n
                z to show number of large blocks detected on conveyor 0 \n
                x to show number of large blocks detected on conveyor 1 \n
                h to see this message again \n");
              break; */
      default: break;
    }

  }
}

void TimerT1Callback(void){ /* Close Gate 0 */
  semGive(Timer0SemID);
}
void TimerT2Callback(void){ /* Close Gate 1 */
  semGive(Timer1SemID);
}
void TimerT3Callback(void){ /* Open Gate 0 */
  semGive(Timer0SemID);
}
void TimerT4Callback(void){ /* Open Gate 1 */
  semGive(Timer1SemID);
}
void TimerT5Callback(void){ /* Close Gate 0 */
  semGive(Timer0SemID);
}
void TimerT6Callback(void){ /* Close Gate 1 */
  semGive(Timer1SemID);
}

void CheckEndSensor0(void){
  resetCountSensor(0);
  printf("Reading count 0 sensor...\n");
  char Count = readCountSensor(0);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor0 ++;
    semGive(EndCountSemID);
    }
}
void CheckEndSensor1(void){
  resetCountSensor(1);
  printf("Reading count 1 sensor...\n");
  char Count = readCountSensor(1);
  if (Count == 1){
    semTake(EndCountSemID, WAIT_FOREVER);
    LargeCountSensor1 ++;
    semGive(EndCountSemID);
  }
}

void MotorController0(void){
  int res;
  char State;
  char DOWN = 0;
  char UP = 1;
  while(1){
    semTake(Timer0SemID, WAIT_FOREVER);
    printf("Got MotorControl 0 semaphore! \n");
    res = msgQReceive(queueMotorC0ID, &State, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 0 message queue! Terminating...");
      exit(0);
    }
    printf("Message received to Motor! \n");
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (State == DOWN){
      switch (CheckGateState){
        case 0 : NextState = 1;
                 setGates(1);
                 break;
        case 1 : NextState = 1;
                 setGates(1);
                 break;
        case 2 : NextState = 3;
                 setGates(3);
                 break;
        case 3 : NextState = 3;
                 setGates(3);
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
      }
    }
    else if (State == UP){
      switch (CheckGateState){
        case 0 : NextState = 0;
                 setGates(0);
                 break;
        case 1 : NextState = 0;
                 setGates(0);
                 break;
        case 2 : NextState = 2;
                 setGates(2);
                 break;
        case 3 : NextState = 2;
                 setGates(2);
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
      }
    }
  printf("Motor 0 state is %c \n", NextState);
  GateState = NextState;
  semGive(MotorStateSemID);

  semTake(BlockTimeSemID, WAIT_FOREVER);
  if (BlockTimePointer0 != 0){
    BlockTimePointer0 = 0;
  }
  semGive(BlockTimeSemID);

  CheckEndSensor0();
  }
}

void MotorController1(void){
  int res;
  char State;
  char DOWN = 0;
  char UP = 1;
  while(1){
    semTake(Timer1SemID, WAIT_FOREVER);
    printf("Got MotorControl 0 semaphore! \n");
    res = msgQReceive(queueMotorC1ID, &State, 1, WAIT_FOREVER);
    if (res == ERROR){
      printf("Error reading sensor 0 message queue! Terminating...");
      exit(0);
    }
    printf("Message received to Motor! \n");
    semTake(MotorStateSemID, WAIT_FOREVER);
    char CheckGateState = GateState;
    char NextState;
    if (State == DOWN){
      switch (CheckGateState){
        case 0 : NextState = 2;
                 setGates(2);
                 break;
        case 1 : NextState = 3;
                 setGates(3);
                 break;
        case 2 : NextState = 2;
                 setGates(2);
                 break;
        case 3 : NextState = 3;
                 setGates(3);
                 break;
        default : NextState = 0;
                  setGates(0);
                 break;
      }
    }
    else if (State == UP){
      switch (CheckGateState){
        case 0 : NextState = 0;
                 setGates(0);
                 break;
        case 1 : NextState = 1;
                 setGates(1);
                 break;
        case 2 : NextState = 0;
                 setGates(0);
                 break;
        case 3 : NextState = 1;
                 setGates(1);
                 break;
        default : NextState = 0;
                  setGates(0);
                  break;
    }
    printf("Motor 1 state is %c \n", NextState);
    GateState = NextState;
    semGive(MotorStateSemID);

    semTake(BlockTimeSemID, WAIT_FOREVER);
    if (BlockTimePointer1 != 0){
      BlockTimePointer1 = 0;
    }
    semGive(BlockTimeSemID);

    CheckEndSensor1();
  }
}
}

void Sendmessage(conveyor, updown){
  int res;
  if (conveyor == 0){
    if (updown == 1){ /* UP */
      res = msgQSend(queueMotorC0ID, &updown, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
      if (res == ERROR){
        printf("Cannot send sensor 1 input into queue! Terminating...");
        exit(0);
      }
    }
    else if (updown == 0){ /* DOWN */
      res = msgQSend(queueMotorC0ID, &updown, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
      if (res == ERROR){
        printf("Cannot send sensor 1 input into queue! Terminating...");
        exit(0);
      }
    }
  }
  else if (conveyor == 1){
    if (updown == 1){ /* UP */
      res = msgQSend(queueMotorC1ID, &updown, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
      if (res == ERROR){
        printf("Cannot send sensor 1 input into queue! Terminating...");
        exit(0);
      }
    }
    else if (updown == 0){ /* DOWN */
      res = msgQSend(queueMotorC1ID, &updown, 1, WAIT_FOREVER, MSG_PRI_NORMAL);
      if (res == ERROR){
        printf("Cannot send sensor 1 input into queue! Terminating...");
        exit(0);
      }
    }
  }
}

void AnalyseConveyor(CurrentState, LastState, Conveyor){
int res;
char up = 1;
char down = 0;
printf("Current state %d", CurrentState);
printf("Laste state %d", LastState);
if (Conveyor == 0){
	printf("Detected on Conveyor 0! \n");
    switch(CurrentState){
      case 0 :
              if (LastState == 1){
                if (BlockTimePointer0 == 0){
                  printf("Setting close gate timer \n");
                  timer_T1_ID = wdCreate();
                  if (timer_T1_ID == NULL) {
                    printf("Cannot create timer!!! Terminating this task...");
                    exit(0);
                  }
                  res = wdStart(timer_T1_ID, 2 * sysClkRateGet(), (FUNCPTR)TimerT1Callback, 0);
          				if (res == ERROR){
          					printf("Cannot start the timer! Terminating... \n");
          					exit(0);
          				}
                  Sendmessage(0, down);
                  printf("Setting open gate timer \n");
                  timer_T3_ID = wdCreate();
                  if (timer_T3_ID == NULL) {
                    printf("Cannot create timer!!! Terminating this task...");
                    exit(0);
                  }
                  res = wdStart(timer_T3_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT3Callback, 0);
        					if (res == ERROR){
          					printf("Cannot start the timer! Terminating... \n");
          					exit(0);
          				}
                  Sendmessage(0, up);
                  semTake(CountSemID, WAIT_FOREVER);
                  SmallCount0 ++;
                  semGive(CountSemID);
                  semTake(BlockTimeSemID, WAIT_FOREVER);
                  BlockTimePointer0 ++;
                  semGive(BlockTimeSemID);
                  }
                else{
                  printf("Setting open gate timer \n");
                  timer_T3_ID = wdCreate();
                  if (timer_T3_ID == NULL) {
                    printf("Cannot create timer!!! Terminating this task...");
                    exit(0);
                  }
                  res = wdStart(timer_T3_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT3Callback, 0);
         					if (res == ERROR){
             				printf("Cannot start the timer! Terminating... \n");
             				exit(0);
           				}
                  Sendmessage(0, up);
                  semTake(CountSemID, WAIT_FOREVER);
                  SmallCount0 ++;
                  semGive(CountSemID);
                 }
               }
               else if (LastState == 3){
                 printf("Second small block detected! \n");
                 semTake(CountSemID, WAIT_FOREVER);
                 SmallCount0 ++;
                 semGive(CountSemID);
                 printf("Setting open gate timer \n");
                 timer_T3_ID = wdCreate();
                 if (timer_T3_ID == NULL) {
                   printf("Cannot create timer!!! Terminating this task...");
                   exit(0);
                 }
                 res = wdStart(timer_T3_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT3Callback, 0);
                 if (res == ERROR){
                   printf("Cannot start the timer! Terminating... \n");
                   exit(0);
                 }
                 Sendmessage(0, up);
               }
               break;
      case 3 : if (LastState != 0){
                 semTake(BlockTimeSemID, WAIT_FOREVER);
                 if (BlockTimePointer1 > 0){
                   printf("Setting open gate timer \n");
                   timer_T3_ID = wdCreate();
                   if (timer_T3_ID == NULL) {
                     printf("Cannot create timer!!! Terminating this task...");
                     exit(0);
                   }
                   res = wdStart(timer_T3_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT3Callback, 0);
                   if (res == ERROR){
                    printf("Cannot start the timer! Terminating... \n");
                    exit(0);
                   }
                   Sendmessage(0, up);
                   printf("Setting close gate timer \n");
                   timer_T5_ID = wdCreate();
                   if (timer_T5_ID == NULL) {
                     printf("Cannot create timer!!! Terminating this task...");
                     exit(0);
                   }
                   res = wdStart(timer_T5_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT5Callback, 0);
                   if (res == ERROR){
                     printf("Cannot start the timer! Terminating... \n");
                     exit(0);
                   }
                   Sendmessage(0, down);
                 }
                 semGive(BlockTimeSemID);
                 semTake(CountSemID, WAIT_FOREVER);
                 printf("Large block detected! \n");
                 LargeCount0 ++;
                 semGive(CountSemID);
               }
               break;
      default :
               break;
    }
  }
else if (Conveyor == 1){
	printf("Detected on Conveyor 1! \n");
  switch(CurrentState){
    case 0 :
            if (LastState == 1){
              if (BlockTimePointer1 == 0){
              	printf("Setting close gate timer");
                timer_T2_ID = wdCreate();
                if (timer_T2_ID == NULL) {
                  printf("Cannot create timer!!! Terminating this task...");
                  exit(0);
                }
                res = wdStart(timer_T2_ID, 2 * sysClkRateGet(), (FUNCPTR)TimerT2Callback, 0);
                if (res == ERROR){
                  printf("Cannot start the timer! Terminating... \n");
                  exit(0);
                }
                Sendmessage(1, down);
                printf("Setting open gate timer \n");
                timer_T4_ID = wdCreate();
                if (timer_T4_ID == NULL) {
                  printf("Cannot create timer!!! Terminating this task...");
                  exit(0);
                }
                res = wdStart(timer_T4_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT4Callback, 0);
                if (res == ERROR){
                  printf("Cannot start the timer! Terminating... \n");
                  exit(0);
                }
                Sendmessage(1, up);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount1 ++;
                semGive(CountSemID);
                semTake(BlockTimeSemID, WAIT_FOREVER);
                BlockTimePointer1 ++;
                semGive(BlockTimeSemID);
              }
              else{
                printf("Setting open gate timer \n");
                timer_T4_ID = wdCreate();
                if (timer_T4_ID == NULL) {
                  printf("Cannot create timer!!! Terminating this task...");
                  exit(0);
                }
                res = wdStart(timer_T4_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT4Callback, 0);
                if (res == ERROR){
                  printf("Cannot start the timer! Terminating... \n");
                  exit(0);
                }
                Sendmessage(1, up);
                semTake(CountSemID, WAIT_FOREVER);
                SmallCount1 ++;
                semGive(CountSemID);
              }
            }
            else if (LastState == 3){
              printf("Second small block detected! \n");
              semTake(CountSemID, WAIT_FOREVER);
              SmallCount1 ++;
              semGive(CountSemID);
              printf("Setting open gate timer \n");
              timer_T4_ID = wdCreate();
              if (timer_T4_ID == NULL) {
                printf("Cannot create timer!!! Terminating this task...");
                exit(0);
              }
              res = wdStart(timer_T4_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT4Callback, 0);
              if (res == ERROR){
                printf("Cannot start the timer! Terminating... \n");
                exit(0);
              }
              Sendmessage(1, up);
            }
            break;
    case 3 : if (LastState != 0){
               semTake(BlockTimeSemID, WAIT_FOREVER);
               if (BlockTimePointer1 > 0){
                 printf("Setting open gate timer \n");
                 timer_T4_ID = wdCreate();
                 if (timer_T4_ID == NULL) {
                   printf("Cannot create timer!!! Terminating this task...");
                   exit(0);
                 }
                 res = wdStart(timer_T4_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT4Callback, 0);
                 if (res == ERROR){
                   printf("Cannot start the timer! Terminating... \n");
                   exit(0);
                 }
                 Sendmessage(1, up);
                 printf("Setting close gate timer \n");
                 timer_T6_ID = wdCreate();
                 if (timer_T6_ID == NULL) {
                   printf("Cannot create timer!!! Terminating this task...");
                   exit(0);
                 }
                 res = wdStart(timer_T6_ID, 3 * sysClkRateGet(), (FUNCPTR)TimerT6Callback, 0);
                 if (res == ERROR){
                   printf("Cannot start the timer! Terminating... \n");
                   exit(0);
                 }
                 Sendmessage(1, down);
               }
               semGive(BlockTimeSemID);
               semTake(CountSemID, WAIT_FOREVER);
               printf("Large block detected! \n");
               LargeCount1 ++;
               semGive(CountSemID);
             }
             break;
    default :
             break;
  }
 }
}

void CheckSensor(){
  char CurrentState0 = 0;
  char CurrentState1 = 0;
  char LastState0 = 0;
  char LastState1 = 0;
  while(1){

    resetSizeSensors(0);
    CurrentState0 = readSizeSensors(0);
    if (CurrentState0 != LastState0){
      AnalyseConveyor(CurrentState0, LastState0, 0);
    }
    LastState0 = CurrentState0;

    resetSizeSensors(1);
    CurrentState1 = readSizeSensors(1);
    if (CurrentState1 != LastState1){
      AnalyseConveyor(CurrentState1, LastState1, 1);
    }
    LastState1 = CurrentState1;
  }
}

void main(void){
  startMotor();
  int CheckSensor_id;
  int MotorController0_id;
  int MotorController1_id;
  int Interface_id;

  BlockTimePointer0 = 0;
  BlockTimePointer1 = 0;

  timer_T1_ID = wdCreate();
  timer_T2_ID = wdCreate();
  timer_T3_ID = wdCreate();
  timer_T4_ID = wdCreate();
  timer_T5_ID = wdCreate();
  timer_T6_ID = wdCreate();

  queueMotorC0ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueMotorC0ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }
  queueMotorC1ID = msgQCreate(100, 1, MSG_Q_PRIORITY);
  if (queueMotorC1ID == NULL){
    printf("Cannot create message queue! Terminating...");
    exit(0);
  }

  MotorStateSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (MotorStateSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  CountSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (CountSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  EndCountSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (EndCountSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  BlockTimeSemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (BlockTimeSemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  Timer0SemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (Timer0SemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  Timer1SemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (Timer0SemID == NULL){
    printf("Cannot create analysis semaphore! Terminating...");
    exit(0);
  }
  /*// Set up tasks*/
  CheckSensor_id = taskSpawn("CheckSensor", 100, 0, 20000,
                      (FUNCPTR)CheckSensor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  Interface_id = taskSpawn("Interface", 102, 0, 20000,
                      (FUNCPTR)Interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  MotorController0_id = taskSpawn("MotorController0", 101, 0, 20000,
                      (FUNCPTR)MotorController0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  MotorController1_id = taskSpawn("MotorController1", 101, 0, 20000,
                      (FUNCPTR)MotorController1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  /*printf("Welcome to block conveyor 4.0! \n" "Press: \n" q to quit \n l to show number of large blocks counted \n s to show number of small blocks detected \n o to show number of small blocks counted \n p to show number of small blocks detected \n k to reset the large block counter \n a to reset the small block counter \n z to show number of large blocks detected on conveyor 0 \n x to show number of large blocks detected on conveyor 1 \n h to see this message again \n ");*/
  taskDelay(60 * sysClkRateGet());
}
