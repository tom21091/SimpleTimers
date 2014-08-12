/**************************************************/
/* SimpleTimer Demo                               */
/**************************************************/
/*    Author: Thomas Kappenman                    */
/*    Copyright 2014, Digilent Inc.               */
/*                                                */
/*   Made for use with chipKIT Uno32              */
/*                                                */
/**************************************************/
/*  Module Description:                           */
/*                                                */
/*    This module implement a demo application    */
/*    of a simplified hardware timer library      */
/*                                                */
/*  Functionality:                                */
/*                                                */  
/*    This library is meant to simplify the setup */
/*    and use of the PIC32 timers, timer          */
/*    interrupts, and hardware PWM implementation */
/*    with the output compare modules.            */
/*                                                */
/**************************************************/
/*  Revision History:                             */
/*                                                */
/*      8/07/2014(TommyK): Created                */
/*                                                */
/**************************************************/

#include <SimpleTimers.h>

#define LD4 13
#define LD5 43
#define OC1pin 3
#define OC2pin 5
#define OC3pin 6
#define OC4pin 9
//#define OC5pin 10; //OC5 requires JP4 jumper on RD4 pin


volatile float dutypercent=0;
volatile bool increment=true;
volatile int seconds=0;

void setup() {
  pinMode(LD5, OUTPUT);
  pinMode(LD4, OUTPUT);
  pinMode(OC1pin, OUTPUT);
  pinMode(OC2pin, OUTPUT);
  pinMode(OC3pin, OUTPUT);
  pinMode(OC4pin, OUTPUT);
  
  //Start timers
  startTimer(TIMER2, 100000);  //.1 second timer
  startTimer(TIMER3, 500);  //.0005 second timer (2000Hz), for PWM
  startTimer(TIMER45, 1000000);  //1 second counter. A 32 bit counter is used since the max period of a 16 bit counter is .2 second
  
  //attaching these timers to interrupts
  attachTimerInterrupt(TIMER2, LD5toggle);
  attachTimerInterrupt(TIMER3, PWMtriangleWave);
  attachTimerInterrupt(TIMER45,  everySecond);
  
  //Pulse width modulation set-up
  //These output a percentage of 3.3V (Dependant on duty cycle, i.e. dutycycle of 10 outputs 10%, or .33V)
  //PWM is used to emulate analog output voltages using digital outputs
  startPWM(TIMER3, OC1, 10); // .33V DC
  startPWM(TIMER3, OC2, 30); // .99V DC
  startPWM(TIMER3, OC3, 60); // 1.98V DC
  startPWM(TIMER3, OC4, 90); // 2.97V DC
}

void loop() {
//Since we're doing everything with timer interrupts, we don't need to do anything here
}


void LD4toggle(){//Toggles LD4
  digitalWrite(13, !digitalRead(13));
}

void LD5toggle(){//Toggles LD5
 digitalWrite(43, !digitalRead(43)); 
}

//We attach timer 45 (period of 1 second) to this function
void everySecond(){
  seconds++;
  if (seconds==10){//At 10 seconds
    attachTimerInterrupt(TIMER2, LD4toggle);//Switch Timer2 interrupt to LD4 toggle
  }
}

//PWM triangle wave: 100 cycles to peak, 100 back down= 200 interrupts at 2000 interrupts per second. Frequency of 10 Hz!
//This function increments the dutycycle to 100
void PWMtriangleWave(){
  if (increment==true){
    if (dutypercent==100){
      dutypercent--;
      increment=false;
    }
    else{
      dutypercent++;
    }
  }
  else {//Increment==false
    if (dutypercent==0){
      dutypercent++;
      increment=true;
    }
    else{
      dutypercent--;
    }
  }
  setDutyCycle(OC2, dutypercent); //OC2 on pin 5 is a PWM triangle wave
}



