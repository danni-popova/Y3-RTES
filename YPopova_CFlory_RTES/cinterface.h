/* Real-Time Embedded Systems
 *
 * School of Energy and Electronic Engineering
 * University of Portsmouth
 *
 * (c) Rinat Khusainov
 */

/*
 * Title: 	cinterface.h
 * Description: Header file for the conveyor interface functions
 * 
 */
#ifndef _CINTERFACE_H_
#define _CINTERFACE_H_

/* Sensor reading functions */
char readSizeSensors(char conveyor);
char readCountSensor(char conveyor);

void resetSizeSensors(char conveyor);
void resetCountSensor(char conveyor);

/* Gate & motor control functions */
void setGates(char state);
void startMotor(void);
void stopMotor(void);

/* Library version */
void cVersion(void);

#endif
