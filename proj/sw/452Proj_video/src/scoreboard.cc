#include "scoreboard.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include <string.h>
#include <stdio.h>

// Screen and VGA definitions
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   1024
#define VGA_BASE_ADDR   0x00900000

//---------------------------------------------------------------------
// Basic Drawing Functions
//---------------------------------------------------------------------
void drawHorizontalLine(int x, int y, int length, int thickness, u32 color) 
{
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < length; i++) {
            u32* vga_ptr = (u32*)(VGA_BASE_ADDR + ((y + t) * SCREEN_WIDTH + (x + i)) * 4);
            *vga_ptr = color;
        }
    }
}

void drawVerticalLine(int x, int y, int length, int thickness, u32 color) 
{
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < length; i++) {
            u32* vga_ptr = (u32*)(VGA_BASE_ADDR + (((y + i) * SCREEN_WIDTH) + (x + t)) * 4);
            *vga_ptr = color;
        }
    }
}


void draw0(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
    // 0
    drawHorizontalLine(x, y, width, lineThickness, boxColor);
    // Draw bottom border
    drawHorizontalLine(x, y + height - lineThickness, width, lineThickness, boxColor);
    // Draw left border
    drawVerticalLine(x, y, height, lineThickness, boxColor);
    // Draw right border
    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}

void draw1(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	//1
	    drawVerticalLine(x + 19, y, height, lineThickness, boxColor);
}

void draw2(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// 2
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawHorizontalLine(x, y + (15 - lineThickness), width, lineThickness, boxColor);
	    drawHorizontalLine(x, y + (29 - lineThickness), width, lineThickness, boxColor);
	    // Draw left border
	    drawVerticalLine(x, y + 14, height/2, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y, height/2, lineThickness, boxColor);
}

void draw3(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// 3
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawHorizontalLine(x, y + ((height/2) - lineThickness), width, lineThickness, boxColor);
	    drawHorizontalLine(x, y + height - lineThickness, width, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}

void draw4(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// 4
	    drawHorizontalLine(x, y +  ((height/2) - lineThickness), width, lineThickness, boxColor);
	    // Draw left border
	    drawVerticalLine(x, y, height/2, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}

void draw5(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// 5
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawHorizontalLine(x, y + ((height/2) - lineThickness), width, lineThickness, boxColor);
	    drawHorizontalLine(x, y + height - lineThickness, width, lineThickness, boxColor);
	    // Draw left border
	    drawVerticalLine(x, y, height/2, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y + height/2 - lineThickness, height/2, lineThickness, boxColor);
}

void draw6(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// 6
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawHorizontalLine(x, y + height/2 - lineThickness, width, lineThickness, boxColor);
	    drawHorizontalLine(x, y + height - lineThickness, width, lineThickness, boxColor);
	    // Draw left border
	    drawVerticalLine(x, y, height, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y + height/2 - lineThickness, height/2, lineThickness, boxColor);
}

void draw7(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// Draw top border
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}

void draw8(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// Draw top border
	    drawHorizontalLine(x, y, width, lineThickness, boxColor);
	    // Draw bottom border
	    drawHorizontalLine(x, y + height/2 - lineThickness, width, lineThickness, boxColor);
	    drawHorizontalLine(x, y + height - lineThickness, width, lineThickness, boxColor);
	    // Draw left border
	    drawVerticalLine(x, y, height, lineThickness, boxColor);
	    // Draw right border
	    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}

void draw9(int x, int y, int width, int height, u32 boxColor, int lineThickness) {
	// Draw top border
		    drawHorizontalLine(x, y, width, lineThickness, boxColor);
		    // Draw bottom border
		    drawHorizontalLine(x, y + height/2 - lineThickness, width, lineThickness, boxColor);
		    // Draw left border
		    drawVerticalLine(x, y, height/2, lineThickness, boxColor);
		    // Draw right border
		    drawVerticalLine(x + width - lineThickness, y, height, lineThickness, boxColor);
}



//---------------------------------------------------------------------
// drawDigitBox: Uses switch-case to choose which digit-drawing function to call.
// x, y specify the top-left of the digit area, and the digit area is 20x30 pixels.
//---------------------------------------------------------------------
void drawDigitBox(char digit, int x, int y, u32 color) {
    int width = 20;
    int height = 30;
    int thickness = 2;
    switch (digit) {
        case '0': draw0(x, y, width, height, color, thickness); break;
        case '1': draw1(x, y, width, height, color, thickness); break;
        case '2': draw2(x, y, width, height, color, thickness); break;
        case '3': draw3(x, y, width, height, color, thickness); break;
        case '4': draw4(x, y, width, height, color, thickness); break;
        case '5': draw5(x, y, width, height, color, thickness); break;
        case '6': draw6(x, y, width, height, color, thickness); break;
        case '7': draw7(x, y, width, height, color, thickness); break;
        case '8': draw8(x, y, width, height, color, thickness); break;
        case '9': draw9(x, y, width, height, color, thickness); break;
        default: break;
    }
}

