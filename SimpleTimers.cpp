/****************************************************************************************/
/*																											*/
/*	SimpleTimers.cpp																				*/
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

#ifndef SIMPLETIMERS_cpp
#define SIMPLETIMERS_cpp

#include "SimpleTimers.h"


//Global ISR function array
volatile static voidFuncPtr intFunc[5];

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	startTimer()
**
**	Parameters:
**		timerNum:	The timer to initialize <TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45> 
**		microseconds:	The period, in microseconds, to set the timer to.
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Starts the specified timer with a period specified by the user in microseconds.
**
**	Example:
**		startTimer(TIMER23, 10000000);	Starts a 32 bit timer (TIMER2 & TIMER3) with a period of 10 seconds
*/
void startTimer(uint8_t timerNum, long microseconds){

	unsigned long cycles = (F_CPU / 1000000) * microseconds;	//Number of cycles = Cycles per second * Seconds
	if (timerNum < 7){
		switch (timerNum){
			case(TIMER1):
				if(cycles < MAX16BIT)              OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_1, cycles-1);           	// No prescale
				else if((cycles >>= 3) < MAX16BIT) OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_8, cycles-1);      	// Prescale by /8 
				else if((cycles >>= 3) < MAX16BIT) OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_64, cycles-1);   	// Prescale by /64
				else if((cycles >>= 2) < MAX16BIT) OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256, cycles-1); 	// Prescale by /256
				else        cycles = 65535, OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256, cycles); 				// Max period
				break;
			case(TIMER2):
				if(cycles < MAX16BIT)             		OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_1, cycles-1);        	// No prescale
				else if((cycles >>= 3) < MAX16BIT) OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_8, cycles-1);         	// Prescale by /8 
				else if((cycles >>= 3) < MAX16BIT) OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_64, cycles-1);   		// Prescale by /64
				else if((cycles >>= 2) < MAX16BIT) OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_256, cycles-1);    	// Prescale by /256
				else        cycles = MAX16BIT - 1, 		OpenTimer2(T2_ON | T2_IDLE_CON | T2_PS_1_256, cycles);	// Max period
				break;
			case(TIMER3):
				if(cycles < MAX16BIT)             		OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_1, cycles-1);         // No prescale
				else if((cycles >>= 3) < MAX16BIT) OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_8, cycles-1);          // Prescale by /8 
				else if((cycles >>= 3) < MAX16BIT) OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_64, cycles-1);   	 // Prescale by /64
				else if((cycles >>= 2) < MAX16BIT) OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_256, cycles-1);     // Prescale by /256
				else        cycles = MAX16BIT - 1, 		OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_256, cycles); // Max period
				break;
			case(TIMER4):
				if(cycles < MAX16BIT)             		OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_1, cycles-1);         // No prescale
				else if((cycles >>= 3) < MAX16BIT) OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_8, cycles-1);          // Prescale by /8 
				else if((cycles >>= 3) < MAX16BIT) OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_64, cycles-1);   	// Prescale by /64
				else if((cycles >>= 2) < MAX16BIT) OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_256, cycles-1);     // Prescale by /256
				else        cycles = MAX16BIT - 1, 		OpenTimer4(T4_ON | T4_IDLE_CON | T4_PS_1_256, cycles); // Max period
				break;
			case(TIMER5):
				if(cycles < MAX16BIT)             		OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_1, cycles-1);           // No prescale
				else if((cycles >>= 3) < MAX16BIT) OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_8, cycles-1);           // Prescale by /8
				else if((cycles >>= 3) < MAX16BIT) OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_64, cycles-1);  	 	// Prescale by /64
				else if((cycles >>= 2) < MAX16BIT) OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_256, cycles-1);      	// Prescale by /256
				else        cycles = MAX16BIT - 1, 		OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_256, cycles); 	// Max period
				break;
			case(TIMER23):
				if(cycles < MAX32BIT)						OpenTimer23(T23_ON | T23_IDLE_CON | T23_PS_1_1, cycles-1);	//No prescale
				else if((cycles >>= 3) < MAX32BIT)	OpenTimer23(T23_ON | T23_IDLE_CON | T23_PS_1_8, cycles-1);	//Prescale by /8
				else if((cycles >>= 3) < MAX32BIT)	OpenTimer23(T23_ON | T23_IDLE_CON | T23_PS_1_64, cycles-1);	//Prescale by /64
				else if((cycles >>= 2) < MAX32BIT)	OpenTimer23(T23_ON | T23_IDLE_CON | T23_PS_1_256, cycles-1);	//Prescale by /256
				else		cycles =MAX32BIT - 1,				OpenTimer23(T23_ON | T23_IDLE_CON | T23_PS_1_256, cycles);	//Max period possible
				break;
			case(TIMER45):
				if(cycles < MAX32BIT)						OpenTimer45(T45_ON | T45_IDLE_CON | T45_PS_1_1, cycles-1);	//No prescale
				else if((cycles >>= 3) < MAX32BIT)	OpenTimer45(T45_ON | T45_IDLE_CON | T45_PS_1_8, cycles-1);	//Prescale by /8
				else if((cycles >>= 3) < MAX32BIT)	OpenTimer45(T45_ON | T45_IDLE_CON | T45_PS_1_64, cycles-1);	//Prescale by /64
				else if((cycles >>= 2) < MAX32BIT)	OpenTimer45(T45_ON | T45_IDLE_CON | T45_PS_1_256, cycles-1);	//Prescale by /256
				else		cycles =MAX32BIT - 1,				OpenTimer45(T45_ON | T45_IDLE_CON | T45_PS_1_256, cycles);	//Max period possible
				break;
		}
	}
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	stopTimer()
**
**	Parameters:
**		timerNum:	The timer to stop <TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45> 
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Stops the specified timer
**
**	Example:
**		stopTimer(TIMER23)	Stops the 32 bit timer (TIMER2 & TIMER3)
*/
void stopTimer(uint8_t timerNum){
	if (timerNum < 7){
		switch(timerNum){
			case TIMER1:CloseTimer1();
				break;
			case TIMER2:CloseTimer2();
				break;
			case TIMER3:CloseTimer3();
				break;
			case TIMER4:CloseTimer4();
				break;
			case TIMER5:CloseTimer5();
				break;
			case TIMER23:CloseTimer23();
				break;
			case TIMER45:CloseTimer45();
				break;
		}
	}
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	setTimerPeriod()
**
**	Parameters:
**		timerNum:	The timer to set the period of <TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45> 
**
**	Return Value:
**		none
**
**	Errors:
**		At the moment, just calls startTimer(), this also enables the timer. This function is
**		mainly for the name.
**
**  Description:
**		Starts the specified timer with the specified period in microseconds
**
**	Example:
**		startTimer(TIMER4, 100000);	Starts a 16 bit timer (TIMER4) with a period of 100 milliseconds.
*/
void setTimerPeriod(uint8_t timerNum, long microseconds){
	startTimer(timerNum, microseconds);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	timerReset()
**
**	Parameters:
**		timerNum:	The timer to reset <TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45> 
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Resets the value of the timer counter to 0.
**
**	Example:
**		timerReset(TIMER23);	Resets the 32 bit timer 2-3 count to 0.
*/
void timerReset(uint8_t timerNum){
	switch(timerNum){
		case TIMER1: WriteTimer1(0);
			break;
		case TIMER2: WriteTimer2(0);
			break;
		case TIMER3: WriteTimer3(0);
			break;
		case TIMER4: WriteTimer4(0);
			break;
		case TIMER5: WriteTimer5(0);
			break;
		case TIMER23: WriteTimer23(0);
			break;
		case TIMER45: WriteTimer45(0);
			break;
	}
}
/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	attachTimerInterrupt()
**
**	Parameters:
**		timerNum:	The timer interrupt to set <TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45> 
**		userfunc:	The interrupt service routine to jump to when the interrupt is triggered
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Specifies a function as an interrupt service routine for a timer.
**
**	Example:
**		attachTimerInterrupt(TIMER23, toggleLED);	Attaches the 32 bit timer 2-3 to the function toggleLED();
*/
void attachTimerInterrupt(uint8_t timerNum, void (*userFunc)(void))
{
    if (timerNum < 7)
    {
		if (timerNum==TIMER23)	timerNum=TIMER3;
		else if (timerNum==TIMER45)	timerNum=TIMER5;
		
        intFunc[timerNum]	=	userFunc;

        switch (timerNum)
        {
            case TIMER1:
                IEC0bits.T1IE     =	0;
                IFS0bits.T1IF     =	0;
				setIntVector(_TIMER_1_VECTOR, (isrFunc) Timer1IntHandler);
                ConfigIntTimer1(T1_INT_ON | _T1_IPL_IPC | _T1_SPL_IPC );
                break;

            case TIMER2:
				IEC0bits.T2IE     =	0;
                IFS0bits.T2IF     =	0;
                setIntVector(_TIMER_2_VECTOR, (isrFunc) Timer2IntHandler);
                ConfigIntTimer2(T2_INT_ON | _T2_IPL_IPC | _T2_SPL_IPC );
                break;

            case TIMER3:
				IEC0bits.T3IE     =	0;
                IFS0bits.T3IF     =	0;
                setIntVector(_TIMER_3_VECTOR, (isrFunc) Timer3IntHandler);
				ConfigIntTimer3(T3_INT_ON | _T3_IPL_IPC | _T3_SPL_IPC );
                break;

            case TIMER4:
				IEC0bits.T4IE     =	0;
                IFS0bits.T4IF     =	0;
                setIntVector(_TIMER_4_VECTOR, (isrFunc) Timer4IntHandler);
				ConfigIntTimer4(T4_INT_ON | _T4_IPL_IPC | _T4_SPL_IPC );
                break;

            case TIMER5:
			    IEC0bits.T5IE     =	0;
                IFS0bits.T5IF     =	0;
                setIntVector(_TIMER_5_VECTOR, (isrFunc) Timer5IntHandler);
				ConfigIntTimer5(T5_INT_ON | _T5_IPL_IPC | _T5_SPL_IPC );
                break;
				
        }
    }
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	detachTimerInterrupt()
**
**	Parameters:
**		timerNum:	The timer interrupt to detach<TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45>
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Detaches the interrupt from the previously specified ISR and disables the timer interrupt
**
**	Example:
**		detachTimerInterrupt(TIMER23);	Detaches timer 2-3 from its ISR, 
*/
void detachTimerInterrupt(uint8_t timerNum)
{
    if (timerNum < 7)
    {
		if (timerNum==TIMER23) timerNum=TIMER3;
		else if (timerNum==TIMER45) timerNum=TIMER5;
        switch (timerNum){
            case TIMER1:
                IEC0bits.T1IE     =	0;
                clearIntVector(_TIMER_1_VECTOR);
                break;
            case TIMER2:
                IEC0bits.T2IE     =	0;
                clearIntVector(_TIMER_2_VECTOR);
                break;
            case (TIMER3):
                IEC0bits.T3IE     =	0;
                clearIntVector(_TIMER_3_VECTOR);
                break;
            case TIMER4:
                IEC0bits.T4IE     =	0;
                clearIntVector(_TIMER_4_VECTOR);
                break;
            case (TIMER5):
                IEC0bits.T5IE     =	0;
                clearIntVector(_TIMER_5_VECTOR);
               break;
		}
        intFunc[timerNum]	=	0;
    }
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	disableTimerInterrupt()
**
**	Parameters:
**		timerNum:	The timer interrupt to disable<TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45>
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Turns off the timer interrupt without detaching the ISR
**
**	Example:
**		disableTimerInterrupt(TIMER4);	Disables timer 4 
*/
void disableTimerInterrupt(uint8_t timerNum)
{
    if (timerNum < 7)
    {
		if (timerNum==TIMER23) timerNum=TIMER3;	
		else if (timerNum==TIMER45) timerNum=TIMER5;
        switch (timerNum){
            case TIMER1:
                IEC0bits.T1IE     =	0;
                break;
            case TIMER2:
                IEC0bits.T2IE     =	0;
                break;
            case (TIMER3):
                IEC0bits.T3IE     =	0;
                break;
            case TIMER4:
                IEC0bits.T4IE     =	0;
                break;
            case (TIMER5):
                IEC0bits.T5IE     =	0;
               break;
		}
    }
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	enableTimerInterrupt()
**
**	Parameters:
**		timerNum:	The timer interrupt to enable<TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER23, TIMER45>
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Turns on the timer interrupt
**
**	Example:
**		enableTimerInterrupt(TIMER4);	Enables timer 4 
*/
void enableTimerInterrupt(uint8_t timerNum)
{
    if (timerNum < 7)
    {
		if (timerNum==TIMER23) timerNum=TIMER3;	
		else if (timerNum==TIMER45) timerNum=TIMER5;
        switch (timerNum){
            case TIMER1:
                IEC0bits.T1IE     =	1;
                break;
            case TIMER2:
                IEC0bits.T2IE     =	1;
                break;
            case (TIMER3):
                IEC0bits.T3IE     =	1;
                break;
            case TIMER4:
                IEC0bits.T4IE     =	1;
                break;
            case (TIMER5):
                IEC0bits.T5IE     =	1;
               break;
		}
    }
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	startPWM
**
**	Parameters:
**		timerNum:	The timer to use, with the period of the PWM <TIMER2, TIMER3, TIMER23>
**		OCnum:	The output compare module to use <OC1, OC2, OC3, OC4, OC5>
**		dutycycle:	The duty cycle (out of 100%) of the PWM
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Enables one of the output compare modules as a PWM with the specified duty cycle,
**		and the period of the specified timer.
**
**	Example:
**		startPWM(TIMER2, OC1, 80);		Initializes OC1 as a PWM output with 80% duty cycle, and TIMER2's period
*/
void startPWM(uint8_t timerNum, uint8_t OCnum, uint8_t dutycycle){
	
	unsigned long outputCompareValue;
	
	if (dutycycle<=100){
		switch(timerNum){
		case TIMER2:
			outputCompareValue = (ReadPeriod2() * dutycycle) / 100;
			switch(OCnum){
			case OC1:
				OpenOC1(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC2:
				OpenOC2(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC3:
				OpenOC3(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC4:
				OpenOC4(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC5:
				OpenOC5(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			}
			break;
		case TIMER3:
			outputCompareValue = (ReadPeriod3() * dutycycle) / 100;
			switch(OCnum){
			case OC1:
				OpenOC1(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC2:
				OpenOC2(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC3:
				OpenOC3(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC4:
				OpenOC4(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC5:
				OpenOC5(OC_ON | OC_IDLE_CON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			}
			break;
		case TIMER23:
			outputCompareValue = (ReadPeriod23() * dutycycle) / 100;
			switch(OCnum){
			case OC1:
				OpenOC1(OC_ON | OC_IDLE_CON | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC2:
				OpenOC2(OC_ON | OC_IDLE_CON | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC3:
				OpenOC3(OC_ON | OC_IDLE_CON | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC4:
				OpenOC4(OC_ON | OC_IDLE_CON | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			case OC5:
				OpenOC5(OC_ON | OC_IDLE_CON | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, outputCompareValue, outputCompareValue);
				break;
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	stopPWM()
**
**	Parameters:
**		OCnum:	The output compare module to turn off <OC1, OC2, OC3, OC4, OC5>
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Turns off the specified output compare module.
**
**	Example:
**		stopPWM(OC1);	Turns off the PWM signal being output by OC1
*/
void stopPWM(uint8_t OCnum){
	switch(OCnum){
		case OC1: CloseOC1();
			break;
		case OC2: CloseOC2();
			break;
		case OC3: CloseOC3();
			break;
		case OC4: CloseOC4();
			break;
		case OC5: CloseOC5();
			break;
	}
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/*	setDutyCycle()
**
**	Parameters:
**		OCnum:	The output compare module to turn off <OC1, OC2, OC3, OC4, OC5>
**		dutycycle:	The duty cycle percent (0-100) to set the PWM to
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**  Description:
**		Sets the duty cycle of the specified output compare module to a certain percent of the current timer's period.
**
**	Example:
**		setDutyCycle(OC3, 90);	Sets the PWM signal on OC3 to 90% duty cycle
*/
void setDutyCycle(uint8_t OCnum, float dutycycle){

	unsigned long outputCompareValue;
	bool timerSrc;
	
	if (dutycycle<=100){
		switch(OCnum){
		case OC1:
			timerSrc = (OC1CON>> _OC1CON_OCTSEL_POSITION) & 1;	//Src = Timer2 if 0, Timer3 if 1
			outputCompareValue = (ReadPeriod2() * !timerSrc + ReadPeriod3() * timerSrc) * dutycycle / 100;
			SetDCOC1PWM(outputCompareValue);
			break;
		case OC2:
			timerSrc = (OC2CON>> _OC2CON_OCTSEL_POSITION) & 1;	//Src = Timer2 if 0, Timer3 if 1
			outputCompareValue = (ReadPeriod2() * !timerSrc + ReadPeriod3() * timerSrc) * dutycycle / 100;
			SetDCOC2PWM(outputCompareValue);
			break;
		case OC3:
			timerSrc = (OC3CON>> _OC3CON_OCTSEL_POSITION) & 1;	//Src = Timer2 if 0, Timer3 if 1
			outputCompareValue = (ReadPeriod2() * !timerSrc + ReadPeriod3() * timerSrc) * dutycycle / 100;
			SetDCOC3PWM(outputCompareValue);
			break;
		case OC4:
			timerSrc = (OC4CON>> _OC4CON_OCTSEL_POSITION) & 1;	//Src = Timer2 if 0, Timer3 if 1
			outputCompareValue = (ReadPeriod2() * !timerSrc + ReadPeriod3() * timerSrc) * dutycycle / 100;
			SetDCOC4PWM(outputCompareValue);
			break;
		case OC5:
			timerSrc = (OC5CON>> _OC5CON_OCTSEL_POSITION) & 1;	//Src = Timer2 if 0, Timer3 if 1
			outputCompareValue = (ReadPeriod2() * !timerSrc + ReadPeriod3() * timerSrc) * dutycycle / 100;
			SetDCOC5PWM(outputCompareValue);
			break;
		}
	}
}


//Interrupt Service Routines
//************************************************************************
// Timer1 ISR
void __attribute__((interrupt(),nomips16)) Timer1IntHandler(void)
{

	if (intFunc[TIMER1] != 0)
	{
		(*intFunc[TIMER1])();
	}
	IFS0bits.T1IF	=	0;
}

//************************************************************************
// Timer2 ISR
void __attribute__((interrupt(),nomips16)) Timer2IntHandler(void)
{

	if (intFunc[TIMER2] != 0)
	{
		(*intFunc[TIMER2])();
	}
	IFS0bits.T2IF	=	0;
}

//************************************************************************
// Timer3 ISR
void __attribute__((interrupt(),nomips16)) Timer3IntHandler(void)
{

	if (intFunc[TIMER3] != 0)
	{
		(*intFunc[TIMER3])();
	}
	IFS0bits.T3IF	=	0;
}

//************************************************************************
// Timer4 ISR
void __attribute__((interrupt(),nomips16))Timer4IntHandler(void)
{

	if (intFunc[TIMER4] != 0)
	{
		(*intFunc[TIMER4])();
	}
	IFS0bits.T4IF	=	0;
}

//************************************************************************
// Timer5 ISR
void __attribute__((interrupt(),nomips16)) Timer5IntHandler(void)
{

	if (intFunc[TIMER5] != 0)
	{
		(*intFunc[TIMER5])();
	}
	IFS0bits.T5IF	=	0;
}

//************************************************************************


#endif

