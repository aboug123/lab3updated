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
#include "..\..\inc\tm4c123gh6pm.h"
#include "buttons.h"


// global variable visible in Watch window of debugger
// increments at least once per button press
uint8_t delay;
volatile uint32_t FallingEdges = 0;
void EdgeCounterPortF_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
	while((SYSCTL_RCGCGPIO_R&0x20) == 0){};
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0x1;
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4 and PF0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4 
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R |= 0x11;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
	
}

void Timer2A_Init100HzInt(void){
  volatile uint32_t delay;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x04;      // activate TIMER2
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // disable TIMER2A during setup
  TIMER2_CFG_R = 0;                // configure for 32-bit timer mode
  // **** TIMER2A initialization ****
                                   // configure for periodic mode
  TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER2_TAILR_R = 400000;//799999;         // start value for 100 Hz interrupts
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// clear TIMER2A timeout flag
  TIMER2_CTL_R |= TIMER_CTL_TAEN;  // enable TIMER2A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // TIMER2A=priority 2
  NVIC_PRI4_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN0_R = 1<<23;              // enable interrupt 19 in NVIC
}

uint8_t buttonVals[10];
uint8_t flag = 0;

//this will disable interrupts every 10ms, to debounce the switch
void Timer2A_Handler(void){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;
	flag = 0;
}

struct Setter setter = {0,0,0,0,0,0,idle};

uint32_t getAlarmHour(void){
	return setter.aHour;
}

uint32_t getAlarmMin(void){
	return setter.aMin;
}

uint8_t getAlarmInit(void){
	return setter.aInit;
}

uint32_t getSetterHour(void){
	return setter.tHour;
}

uint32_t getSetterMin(void){
	return setter.tMin;
}

uint8_t getSetterInit(void){
	return setter.tInit;
}



void GPIOPortF_Handler(void){
	if(flag == 0){
		flag = 1;
		
		if((GPIO_PORTF_DATA_R & 0x10)){
		
			switch(setter.st){
				case idle:
					setter.tInit = 0;
					setter.st = set;
				break;
						
				case set:
						setter.st = (setter.aInit)? hours : tHours;
					break;
						
				case hours:
							setter.st = minutes;
					break;
				case minutes:
							setter.st = tHours;
						break;
						
				case tHours:
							setter.st = tMinutes;
					 break;
					 
				case tMinutes:
							setter.st = idle;
					 break;	
					
					}
				} else {
					button2Input(1);
				}
		
				
			}	
	GPIO_PORTF_ICR_R = 0x11;      // acknowledge flag4
	
}


void button2Input(uint8_t buttonPress){
	if(buttonPress) {
		switch(setter.st){
			case idle:
			break;
					
			case set:
					setter.aInit ^= 1;
				break;
					
			case hours:
						setter.aHour = (setter.aHour +1) %24;
					break;
				
			case minutes:
						setter.aMin = (setter.aMin +1) %60;
					break;
					
			case tHours:
					setter.tHour = (setter.tHour +1) %60;
					setter.tInit = 1;
				 break;
				 
			case tMinutes:
					setter.tMin = (setter.tMin +1) %60;
					setter.tInit = 1;
				 break;	
			}
		}
}

