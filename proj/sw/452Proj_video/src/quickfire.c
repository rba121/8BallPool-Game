#include "quickfire.h"
#include "scoreboard.h"
#include "bonustimer.h"
#include "sleep.h"
#include "render.h"
#include "movements.h"
#include "btn_interrupts.h"

int TIMER_START_VALUE = 90;


static void drawColon(int x, int y, int width, int height, u32 boxColor, int lineThickness) 
{
    // Draw top dot of colon
    drawVerticalLine(x + width/2 - lineThickness/2, y + height/4 - lineThickness/2,
                    lineThickness*2, lineThickness, boxColor);

    // Draw bottom dot of colon
    drawVerticalLine(x + width/2 - lineThickness/2, y + 3*height/4 - lineThickness/2,
                    lineThickness*2, lineThickness, boxColor);
}

void clear_timer_digits(int x, int y)
{
    int digitWidth = 20;
    int digitHeight = 30;
    int digitSpacing = 3;
    int colonWidth = 20;

    clear_sprite(image_buffer, pooltable_image, digitWidth, digitHeight, x, y);

    clear_sprite(image_buffer, pooltable_image, colonWidth, digitHeight,
                x + digitWidth + digitSpacing, y);

    clear_sprite(image_buffer, pooltable_image, digitWidth, digitHeight,
                x + 2*(digitWidth + digitSpacing), y);
    clear_sprite(image_buffer, pooltable_image, digitWidth, digitHeight,
                x + 3*(digitWidth + digitSpacing), y);
}

//to add extra time when player pockets ball
void bonus_time(int seconds)
{
    if (!ball_hit)
        return;

    uint32_t currentTime = Timer_GetValue();
    uint32_t newTime = currentTime + seconds;

    Timer_Reset(newTime);
    Timer_Start();

    // Display time immediately
    int minutes = newTime / 60;
    int seconds_remaining = newTime % 60;
    displayTimer(minutes, seconds_remaining, 1020, 72, 0xFFFFFF);

    displayTimer(minutes, seconds_remaining, 1020, 72, 0x00FF00);  
    usleep(50000);
    displayTimer(minutes, seconds_remaining, 1020, 72, 0xFFFFFF); 
}

void displayTimer(int minutes, int seconds, int x, int y, u32 color) 
{

	clear_timer_digits(x,y);

    char minuteChar = '0' + minutes;
    char secondsTensChar = '0' + (seconds / 10);
    char secondsOnesChar = '0' + (seconds % 10);

    int digitWidth = 20;
    int digitHeight = 30;
    int digitSpacing = 3;

    //displaying minutes
    drawDigitBox(minuteChar, x, y, color);

    //displaying colons
    drawColon(x + digitWidth + digitSpacing, y, digitWidth, digitHeight, color, 2);
    
    //displaying seconds
    drawDigitBox(secondsTensChar, x + 2*(digitWidth + digitSpacing), y, color);
    drawDigitBox(secondsOnesChar, x + 3*(digitWidth + digitSpacing), y, color);

    Xil_DCacheFlush();
}


void Timer_feature()
{
	static bool flag = false;
	static int frame_counter = 0;

    //Start Timer on hit 
	if(ball_hit && !flag)
	{
		Timer_Reset(TIMER_START_VALUE);
		Timer_Start();
		flag = true;
	}

    
	if(flag)
	{
		frame_counter++;

		uint32_t currentTime = Timer_GetValue();

        //Displaying time based on frame counter
		if (frame_counter >= 10 || currentTime == 0)
		{
			   frame_counter = 0;

			int minutes = currentTime / 60;
			int seconds = currentTime % 60;
			displayTimer(minutes, seconds, 1020, 72, 0xffffff);

            //As timer goes zero, switch to MENU STATE 
			if(currentTime == 0)
			{
				Timer_Stop();
				displayTimer(0, 0, 1020, 72,  0xffffff);
				currentState = MENU_STATE;
				state_counter = 0;
				flag = false;
			}
		}
	}
	else
	{
        //Initial Time when timer hasn't started
		int minutes = TIMER_START_VALUE / 60;
		int seconds = TIMER_START_VALUE % 60;
		displayTimer(minutes, seconds, 1020, 72, 0xffffff);
	}


}
