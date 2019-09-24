// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdio.h>
#include "..\..\inc\ADCSWTrigger.h"
#include "..\..\inc\ST7735.h"
#include "..\..\inc\PLL.h"
#include "..\..\inc\tm4c123gh6pm.h"
#include "bool.h"
#include "speaker.h"
#include "buttons.h"
#include "ClockDisplay.h"

#define PF3	            (*((volatile uint32_t *)0x40025012))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

uint32_t gTime = 0;
uint32_t currentsecs, currentmin, currenthour = 0 ;

void Timer0A_Init(uint32_t period){
  volatile uint32_t delay;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = period-1;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
	
	
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

void Timer0A_Handler(void) {
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	
	if(getSetterInit()){
		currenthour = getSetterHour();
		currentmin = getSetterMin();
		currentsecs = 0;
		gTime = (currenthour * 3600) + (currentmin * 60);
	} else {	
		gTime = (gTime + 1) % 86400;
		currentsecs = gTime % 60;
		currenthour = gTime / 3600;
		currentmin = (gTime-(3600*(gTime/3600)))/60;
	}
	
	clearHands();
	setTime(currenthour, currentmin, currentsecs);
	displayTime();
	
	if(getAlarmInit() && (getAlarmHour() == currenthour) && (getAlarmMin() == currentmin)){
	  EnableSpeaker(1);
	} else {
		EnableSpeaker(0);
	}
}


int main(){
	
  DisableInterrupts();
	clockDisplayInit();
	Timer0A_Init(80000000); // one second interrupts
	Timer2A_Init100HzInt();
	SpeakerInit(2000, 1, 75);
	EdgeCounterPortF_Init();
	ADC0_InitSWTriggerSeq3_Ch9();         
	ADC0_SAC_R |= 0x4;
	EnableInterrupts();
	
	
	while(1){
		WaitForInterrupt();
	}	
}


