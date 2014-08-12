/****************************************************************************************/
/*																											*/
/*	SimpleTimers.h																					*/
/*                                                                                                     		*/
/*	This library provides a simple interface to control the timers on				*/
/*		the PIC32, as well as timer interrupts and PWM								*/
/*																											*/
/***************************************************************************************/
/*	Authors: 	Thomas Kappenman 															*/
/*	Copyright 2014, Digilent Inc.																*/
/***************************************************************************************/
/*  Module Description: 																			*/
/*																											*/
/*	This library provides functions that set up the timers on the PIC32	    	*/
/*	microcontroller. It also provides functions to link timer interrupts to 		*/
/* other functions, and provides an easy way to set up a basic PWM	    	*/
/* signal using output compare modules.													*/
/*																											*/
/**************************************************************************************/
/*  Revision History:																				*/
/*																											*/
/*		8/7/2014(TommyK): Created															*/
/*																											*/
/***************************************************************************************/
/*																											*/
/*		This module was adapted from the PIC32 core library WInterrupts.c	*/
/*		by Mark Sproul and Gene Apperson.												*/
/*																											*/
/***************************************************************************************/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SIMPLETIMERS_h
#define SIMPLETIMERS_h

#include <peripheral/timer.h>
#include <peripheral/outcompare.h>
#include <p32xxxx.h>
#include "pins_arduino.h"
#include "wiring_private.h"

//#include "WConstants.h"
//#include <sys/attribs.h>
//#include "p32_defs.h"

//Symbols for referring to Timer interrupts
#define TIMER1 0
#define TIMER2	1
#define TIMER3	2
#define TIMER4	3
#define TIMER5	4
#define TIMER23	5
#define TIMER45	6

//Symbols for output compare modules
#define OC1	1
#define OC2	2
#define OC3	3
#define OC4	4
#define OC5	5

//Max timer counter values
#define MAX16BIT 65536
#define MAX32BIT 4294967296

// forward references to the ISRs
void __attribute__((interrupt(),nomips16)) Timer1IntHandler(void);
void __attribute__((interrupt(),nomips16)) Timer2IntHandler(void);
void __attribute__((interrupt(),nomips16)) Timer3IntHandler(void);
void __attribute__((interrupt(),nomips16)) Timer4IntHandler(void);
void __attribute__((interrupt(),nomips16)) Timer5IntHandler(void);

//Forward references to library functions
void startTimer(uint8_t timerNum, long microseconds);
void stopTimer(uint8_t timerNum);
void setTimerPeriod(uint8_t timerNum, long microseconds);
void timerReset(uint8_t timerNum);
void attachTimerInterrupt(uint8_t timerNum, void (*userFunc)(void));
void detachTimerInterrupt(uint8_t timerNum);
void disableTimerInterrupt(uint8_t timerNum);
void enableTimerInterrupt(uint8_t timerNum);

void startPWM(uint8_t timerNum, uint8_t OCnum, uint8_t dutycycle);
void stopPWM(uint8_t OCnum);
void setDutyCycle(uint8_t OCnum, float dutycycle);




#endif
