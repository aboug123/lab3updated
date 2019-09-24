// EdgeInterruptPortF.c
// Runs on LM4F120 or TM4C123
// Request an interrupt on the falling edge of PF4 (when the user
// button is pressed) and increment a counter in the interrupt.  Note
// that button bouncing is not addressed.
// Daniel Valvano
// August 30, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019
   Volume 2, Program 5.6, Section 5.5

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// user button connected to PF4 (increment counter on falling edge)

#include <stdint.h>
#include "..\..\ValvanoWaveTM4C123v5\inc\tm4c123gh6pm.h"


// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t FallingEdges = 0;
void EdgeCounterPortF_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4 and PF0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F000F; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
	
}

uint32_t old_alarmhour, old_alarmmin, current_alarmhour, current_alarmmin = 0;

enum AlarmSet {hours, minutes, idle} ;
typedef enum AlarmSet AlarmSet_t;

struct Alarm{
	uint32_t hour ;
	uint32_t min;
	AlarmSet_t st ;
}
;

struct Alarm alarm = {0,0,idle};



void GPIOPortF_Handler(void){
  
	
  FallingEdges = FallingEdges + 1;

	
	switch(alarm.st)
	{
		case idle:
				if( GPIO_PORTF_DATA_R == 0x10 ){
					alarm.st = hours;
				}
			break;
				
				
		case hours:
				if( GPIO_PORTF_DATA_R == 0x10 ){
					alarm.st = minutes;
				}
				if( GPIO_PORTF_DATA_R == 0x01) {
					current_alarmhour = (current_alarmhour +1) %24;
				}
				break;
			
		case (minutes):
			
				if(GPIO_PORTF_DATA_R == 0x10) {
					alarm.st = idle;
				}
				if( GPIO_PORTF_DATA_R == 0x01) {
					current_alarmmin = (current_alarmmin +1) %60;
				}
				break;
			
			}
	GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
	
}


