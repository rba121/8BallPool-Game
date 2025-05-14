#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "xil_types.h"

void drawHorizontalLine(int x, int y, int length, int thickness, u32 color);
void drawVerticalLine(int x, int y, int length, int thickness, u32 color);

//draw functions for digits
void draw0(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw1(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw2(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw3(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw4(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw5(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw6(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw7(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw8(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void draw9(int x, int y, int width, int height, u32 boxColor, int lineThickness);
void drawDigitBox(char digit, int x, int y, u32 color);



#endif // SCOREBOARD_H
