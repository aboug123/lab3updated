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
#include "..\..\ValvanoWaveTM4C123v5\inc\ADCSWTrigger.h"
#include "..\..\ValvanoWaveTM4C123v5\inc\ST7735.h"
#include "..\..\ValvanoWaveTM4C123v5\inc\PLL.h"
#include "..\..\ValvanoWaveTM4C123v5\inc\tm4c123gh6pm.h"
#include "bool.h"
#include "timers_init.h"
#include "alarm.h"
#include "speaker.h"





#define PF3	            (*((volatile uint32_t *)0x40025012))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

#define CLK_PERIOD 10 //ms
#define ADC_RANGE 4096
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


uint32_t alarm_hours = 0;
uint32_t alarm_minutes= 0;
bool timersec = false;						
bool timerhour = false;						
bool timermin = false;					
uint32_t currenthour = 0;						
uint32_t currentsecs = 0;						
uint32_t currentmin = 0;						
uint32_t oldhour = 0;										
uint32_t oldsecs = 0;										
uint32_t oldmin = 0;			

int main(){
	
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Timer0A_Init( 80000000); // one second interrupts

	EnableInterrupts();
	set_alarm_time();
	
	while(1){
		
		PF2^=0x04;
		
		if(timerhour == true) {
			// print out the hour time on screen
			timerhour = false;
		}
		
		if(timermin == true) {
			// print out the hour time on screen
			timermin = false;
		}
			
		if(timersec == true) {
			// print out the hour time on screen
			timersec = false;
		}
		
		set_alarm_time();
		alarm_hours = get_alarmhour();
		alarm_minutes = get_alarmmin();
		
		currenthour = get_timehour();
		currentmin = get_timemin();
		currentsecs = get_timesec();
		
		if( currentmin == alarm_minutes && currenthour == alarm_hours){
			enablespeaker(true);
			
		}
		
		else{
			enablespeaker (false);
			
		}
		
	
		
		
		
		
		
	}	
}


