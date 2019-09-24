#include <stdint.h>
#include <stdio.h>
#include "../../inc/ADCSWTrigger.h"
#include "../../inc/ST7735.h"
#include "../../inc/PLL.h"
#include "tm4c123gh6pm.h"
#include "bool.h"
//#include "timers_init.h" keep getting an error to include because it doesnt recognize unit_32t

uint32_t alarm_hours = 0;
uint32_t alarm_minutes= 0;


void set_alarm_time(void){
	//ST7735_SetCursor(0,3);
	alarm_hours = 0;
	alarm_minutes = 1;
	//printf("%02d:%02d", alarm_hours, alarm_minutes);
}

uint32_t get_alarmhour(void){
	
	return alarm_hours ;
	
}

uint32_t get_alarmmin(void){
	
	return alarm_minutes;
	
}

