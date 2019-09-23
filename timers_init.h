#include <stdint.h>

void TIMER2A_Init(void);

void Timer0A_Init(uint32_t period);

void Timer1A_Init(void);

uint32_t get_timemin(void);
uint32_t get_timesec(void);
uint32_t get_timehour(void);
	