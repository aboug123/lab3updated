#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "ClockDisplay.h"
#include "buttons.h"
#include "../../inc/ST7735.h"
#include "../../inc/PLL.h"

const int16_t fxpSine[60] = {
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
995
};

const int16_t fxpCosine[60] = {
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
-105
};

enum TimeStyle {AM, PM};
typedef enum TimeStyle TimeStyle_t;

enum HandStyle {Seconds, Minutes, Hours};
typedef enum HandStyle HandStyle_t;

struct AnalogClock {
	int32_t rawSec, rawMin, rawHour;
	int32_t sec[2];
	int32_t min[2];
	int32_t hour[2];
	TimeStyle_t st;
};

typedef struct AnalogClock AnalogClock_t;
AnalogClock_t clock;

void clockDisplayInit(void){
	PLL_Init(Bus80MHz);
	Output_Init();
	ST7735_XYplotInit("Alarm Clock", -1500, 1500, -1500, 1500, ST7735_WHITE);
	drawClock(1300);
	
	clock.sec[0] = 0;
	clock.sec[1] = 0;
	clock.min[0] = 0;
	clock.min[1] = 0;
	clock.hour[0] = 0;
	clock.hour[1] = 0;
}

void setTime(int32_t hour, int32_t min, int32_t sec){
	clock.rawSec = sec;
	clock.rawMin = min;
	clock.rawHour = hour;
	
	clock.sec[0] = (int32_t) fxpCosine[sec];
	clock.sec[1] = (int32_t) fxpSine[sec];
	
	int32_t first = (int32_t) fxpCosine[min];
	int32_t next = (int32_t) fxpCosine[(min+1)%60];
	clock.min[0] = (((next - first) * sec) / 60) + first;
	
	first = (int32_t) fxpSine[min];
	next = (int32_t) fxpSine[(min+1)%60];
	clock.min[1] = ((next - first) * sec) / 60  + first;
	
	first = (int32_t) fxpCosine[5*(hour%12)];
	next = (int32_t) fxpCosine[5*((hour+1)%12)];
	clock.hour[0] = ((next - first) * min) / 60 + first;
	
	first = (int32_t) fxpSine[5*(hour%12)];
	next = (int32_t) fxpSine[5*((hour+1)%12)];
	clock.hour[1] = ((next - first) * min) / 60 + first;
	
	clock.st = (hour >= 12)? PM : AM;
}

void drawHand(int32_t plotX, int32_t plotY, int16_t color, HandStyle_t type){
	int32_t bufX[30];
	int32_t bufY[30];
	
	for(int32_t i = 0; i < 30; i++){
		bufX[i] = (plotX*i)/30;
		bufY[i] = (plotY*i)/30;
	}
	
	if(type != Hours)
		ST7735_XYplot(30, bufX, bufY, color);
	else
		ST7735_XYplot(18, bufX, bufY, color);
	
}

void displayTime(void){
	drawHand(clock.sec[0], clock.sec[1], ST7735_RED, Seconds);
	drawHand(clock.min[0], clock.min[1], ST7735_BLACK, Minutes);
	drawHand(clock.hour[0], clock.hour[1], ST7735_BLACK, Hours);
	
	
	char buffer[5];
	
	ST7735_SetCursor(0,0);
	ST7735_OutString("Time: ");
	
	if(clock.rawHour == 0)
		sprintf(buffer, "%02d:%02d ", 12, clock.rawMin);
	else
		sprintf(buffer, "%02d:%02d ", clock.rawHour%12, clock.rawMin);
	
	ST7735_OutString(buffer);
	
	if(clock.rawHour > 11)
		ST7735_OutString("PM\n");
	else
		ST7735_OutString("AM\n");
	
	ST7735_OutString("Alarm: ");
	if(getAlarmInit()){
		if(getAlarmHour() == 0)
			sprintf(buffer, "%02d:%02d ", 12, getAlarmMin());
		else
			sprintf(buffer, "%02d:%02d ", getAlarmHour()%12, getAlarmMin());
		
		ST7735_OutString(buffer);
		
		if(getAlarmHour() > 11)
			ST7735_OutString("PM\n");
		else
			ST7735_OutString("AM\n");
	} else {
		ST7735_OutString("Disabled");
	}
	
}

void clearHands(void){
	drawHand(clock.sec[0], clock.sec[1], ST7735_WHITE, Seconds);
	drawHand(clock.min[0], clock.min[1], ST7735_WHITE, Minutes);
	drawHand(clock.hour[0], clock.hour[1], ST7735_WHITE, Hours);
}

void drawClock(int32_t clkRadius){
	int32_t screenX;
	int32_t screenY;
	char buf[2];
	
	for(int i = 1; i < 13; i++){
		screenX = ST7735_XGetScreenPosition((1300 * (int32_t) fxpCosine[5*(i%12)]) / 1000);
		screenY = ST7735_YGetScreenPosition((1300 * (int32_t) fxpSine[5*((i+6)%12)]) / 1000);
		
		sprintf(buf, "%d", i);
		
		ST7735_DrawChar(screenX - 5, screenY, buf[0], ST7735_BLACK, ST7735_WHITE, 1);
		if(i > 9)
			ST7735_DrawChar(screenX, screenY, buf[1], ST7735_BLACK, ST7735_WHITE, 1);
	}
}
