#include <stdint.h>

void clockDisplayInit(void);
void setTime(int32_t hour, int32_t min, int32_t sec);
void displayTime(void);
void clearHands(void);
void drawClock(int32_t clkRadius);

