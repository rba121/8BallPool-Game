#ifndef QUICKFIRE_H
#define QUICKFIRE_H

#include "xil_cache.h"
#include "xil_printf.h"
#include <string.h>
#include <stdio.h>

//To display timer on screen
void displayTimer(int minutes, int seconds, int x, int y, u32 color);

//Function for time functionality
void Timer_feature();

//Function to clear digits as time decrementing
void clear_timer_digits(int x, int y);

//Function to add bonus time when ball is pocketed
void bonus_time(int seconds);


#endif
