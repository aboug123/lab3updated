//#include "digital_clock.h"
#include "..\..\inc\ST7735.h"
#include "../inc/PLL.h"
#include "tm4c123gh6pm.h"

#define radius 1000

//for the clock we need 60 points around a circle,
//in fixed point with resolution of 0.001
const int32_t sin[60] = {
0,
105,
208,
309,
407,
500,
588,
669,
743,
809,
866,
914,
951,
978,
995,
1000,
995,
978,
951,
914,
866,
809,
743,
669,
588,
500,
407,
309,
208,
105,
0,
-105,
-208,
-309,
-407,
-500,
-588,
-669,
-743,
-809,
-866,
-914,
-951,
-978,
-995,
-1000,
-995,
-978,
-951,
-914,
-866,
-809,
-743,
-669,
-588,
-500,
-407,
-309,
-208,
-105};

//same for cosine
const int32_t cos[60] = {
1000,
995,
978,
951,
914,
866,
809,
743,
669,
588,
500,
407,
309,
208,
105,
0,
-105,
-208,
-309,
-407,
-500,
-588,
-669,
-743,
-809,
-866,
-914,
-951,
-978,
-995,
-1000,
-995,
-978,
-951,
-914,
-866,
-809,
-743,
-669,
-588,
-500,
-407,
-309,
-208,
-105,
0,
105,
208,
309,
407,
500,
588,
669,
743,
809,
866,
914,
951,
978,
995};

extern volatile uint32_t seconds;

struct clock {
	int32_t sec[2];
	int32_t minu[2];
	int32_t hour[2];
};

struct clock analogClock;

void setTime(uint32_t hour, uint32_t min, uint32_t sec){
	analogClock.sec[0] = sin[sec];
	analogClock.sec[1] = cos[sec];
	
	analogClock.minu[0] = ((sin[(minu+1) % 60] - sin[minu])*sec)/60 + sin[minu];
	analogClock.minu[1] = ((cos[(minu+1) % 60] - cos[minu])*sec)/60 + cos[minu];
	
	analogClock.hour[0] = ((sin[(hour+1) % 60] - sin[hour])*sec)/60 + sin[hour];
	analogClock.hour[1] = ((cos[(hour+1) % 60] - cos[hour])*sec)/60 + cos[hour];
}

void displayTime(void){
	ST7735_XYplotInit("Alarm Clock", -1500, 1500, -1500, 1500, ST7735_WHITE);
	
	int32_t handX[100];
	int32_t handY[100];
	//there are three hands to draw, second, minute, and hour
	//the second hand
	for(uint8_t i = 0; i < 100; i++){
		handY[i] = ((i+1)*analogClock.sec[0]) / 100;
		handX[i] = ((i+1)*analogClock.sec[1]) / 100;
	}
	ST7735_XYplot(100, handX, handY, ST7735_RED);
	//the minute hand
	for(uint8_t i = 0; i < 100; i++){
		handY[i] = ((i+1)*analogClock.minu[0]) / 100;
		handX[i] = ((i+1)*analogClock.minu[1]) / 100;
	}
	ST7735_XYplot(100, handX, handY, ST7735_BLACK);
	//the hour hand, smaller than the other 2
	for(uint8_t i = 0; i < 100; i++){
		handY[i] = ((i+1)*analogClock.hour[0]) / 60;
		handX[i] = ((i+1)*analogClock.hour[1]) / 60;
	}
	ST7735_XYplot(100, handX, handY, ST7735_BLACK);
	
	ST7735_OutString("Alarm set for: ");
	ST7735_OutString(alarmString);
}

