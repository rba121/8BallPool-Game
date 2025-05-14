#include "bonustimer.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_printf.h"

//Function to start timer to count down
void Timer_Start()
{
    Xil_Out32(TIMER_BASEADDR + TIMER_CONTROL_REG, 0x1);
}

//Function to pause the timer 
void Timer_Stop()
{
    Xil_Out32(TIMER_BASEADDR + TIMER_CONTROL_REG, 0x0);
}

//Function to trigger the new value in timer
void Timer_Reset(uint32_t value)
{
    Xil_Out32(TIMER_BASEADDR + TIMER_SET_REG, value);
    Xil_Out32(TIMER_BASEADDR + TIMER_CONTROL_REG, 0x2);
}

//Function to read the last set value of timer
uint32_t Timer_GetValue()
{
    return Xil_In32(TIMER_BASEADDR + TIMER_SET_REG);
}

//Reads the actual current value
uint32_t GetValue()
{
    return Xil_In32(TIMER_BASEADDR + TIMER_VALUE_REG);
}




