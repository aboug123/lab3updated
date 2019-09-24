#include <stdint.h>
#include <stdio.h>
#include "../../inc/tm4c123gh6pm.h"
#include "bool.h"
#include "timers_init.h"


#define PF3	            (*((volatile uint32_t *)0x40025012))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
	
uint32_t currentsecs, currentmin, currenthour, oldsecs, oldmin, oldhour, timersec, timermin, timerhour = 0 ;

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
	
	PF1 ^=0x02;
	oldsecs = currentsecs;
	currentsecs = currentsecs +1;
	timersec = true;
	
	if (currentsecs >= 60){
		currentsecs = 0;
		oldmin = currentmin;
		currentmin = currentmin +1;
		timermin = true;
	}
	if (currentmin >= 60){
		currentmin = 0;
		oldhour = currenthour;
		currenthour = currenthour +1;
		timerhour = true;
	}
	if (currenthour >= 12){
		
		currenthour = 0;
		
	}
	
	PF1 ^=0x02;
	
	
}

	uint32_t get_timemin(void){
		
		return currentmin;
	}
	
	uint32_t get_timesec(void){
		return currentsecs;
		
	}
		
  uint32_t get_timehour(void){
		return currenthour;
	}


void Timer1_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER1_TAILR_R = 0x00000320;  // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
	TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
	
	
}

