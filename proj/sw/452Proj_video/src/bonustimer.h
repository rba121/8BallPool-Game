#ifndef BONUS_H
#define BONUS_H

#include <stdint.h>

#define TIMER_BASEADDR  0x43C10000
#define TIMER_CONTROL_REG     0x00 //reg0
#define TIMER_SET_REG         0x04 //reg1
#define TIMER_VALUE_REG       0x08 //reg2
#define BONUS_INCR_REG        0x0C //reg3

//Function to start timer to count down
void Timer_Start();

//Function to pause the timer 
void Timer_Stop();

//Function to trigger the new value in timer
void Timer_Reset(uint32_t value);

//Function to read the last set value of timer
uint32_t Timer_GetValue();

//Reads the actual current value
uint32_t GetValue();


#endif
