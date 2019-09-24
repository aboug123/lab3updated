// EdgeInterruptPortF.h
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

enum SetStates {set, hours, minutes, tHours, tMinutes, idle} ;
typedef enum SetStates SetStates_t;

struct Setter{
	uint32_t aHour;
	uint32_t aMin;
	uint8_t aInit;
	uint32_t tHour;
	uint32_t tMin;
	uint8_t tInit;
	SetStates_t st;
};


void EdgeCounterPortF_Init(void);
void Timer2A_Init100HzInt(void);
uint32_t getAlarmHour(void);
uint32_t getAlarmMin(void);
uint8_t getAlarmInit(void);
uint32_t getSetterHour(void);
uint32_t getSetterMin(void);
uint8_t getSetterInit(void);
void button2Input(uint8_t);

