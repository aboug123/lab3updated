#include <stdint.h>
#include <stdlib.h>
#include "../inc/tm4c123gh6pm.h"
#include "Speaker.h"

#define MHZ_80 80000000
#define PD1 (*((volatile uint32_t*)0x40007008))
	
uint32_t counter = 0;
uint32_t speakerFreq = 0;
uint8_t periodFactor = 0;
uint8_t speakerDutyCycle = 0;
uint8_t speakerEn = 0;

void (*PeriodicTask)(void);   // user function

// ***************** Timer3_Init ****************
// Activate Timer3 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer3_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
  PeriodicTask = task;          // user function
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
  (*PeriodicTask)();                // execute user task
}

void EnableSpeaker(uint8_t start){
	speakerEn = start;
}

void SpeakerPlay(void){
	if(speakerEn){
		counter = (counter + 1) % (periodFactor*speakerFreq);
		
		if(counter < (speakerDutyCycle*periodFactor*speakerFreq) / 100)
			PD1 ^= 0x02;
		else
			PD1 &= ~0x02;
	}
}

void SpeakerInit(uint32_t freq, uint8_t periodInSecs, uint8_t dutyCycle){
	speakerFreq = freq;
	periodFactor = periodInSecs;
	speakerDutyCycle = dutyCycle;
	
	Timer3_Init(&SpeakerPlay, (unsigned long) (MHZ_80 / freq));
	
	//Port D init
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_RCGCGPIO_R&0x08) == 0){};
	GPIO_PORTD_DIR_R |= 0x02;	
	GPIO_PORTD_AFSEL_R &= ~0x02;
	GPIO_PORTD_AMSEL_R &= ~0x02;
	GPIO_PORTD_PCTL_R &= ~0x000000F0;
	GPIO_PORTD_DEN_R |= 0x02;	
}
