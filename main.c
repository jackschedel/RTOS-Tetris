// Lab 3, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 3 is intended to introduce you to RTOS concepts. In this, you will
// - configure the systick function
// - write asm functions for context switching
// - write semaphore functions
// - write scheduler functions to add threads / run scheduling algorithms
// - write critical section assembly functions

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"
#include <driverlib/uart.h>
#include <driverlib/uartstdio.h>

#include "./threads.h"
#include "./G8RTOS/G8RTOS_IPC.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void Multimod_Init()
{
    UART_Init();
    BMI160_Init();
    OPT3001_Init();
    //LaunchpadLED_Init();
    LaunchpadButtons_Init();
    ST7789_Init();
    JOYSTICK_Init();
    MultimodButtons_Init();
}

/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

// Be sure to add in your source files from previous labs into "MultimodDrivers/src/"!
// If you made any modifications to the corresponding header files, be sure to update
// those, too.
int main(void)
{
    // Set system clock speed
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    G8RTOS_Init(Idle_Thread);

    Multimod_Init();
    UART_Init();

    UARTprintf("\n-----------\nSystem Restarted - UART Online.\n\n");

    // Add threads, initialize semaphores here!
    G8RTOS_InitSemaphore(&sem_UART, 1);
    G8RTOS_InitSemaphore(&sem_buttons, 0);
    G8RTOS_InitSemaphore(&sem_jumping, 0);
    G8RTOS_InitSemaphore(&sem_died, 0);
    G8RTOS_InitSemaphore(&sem_reset, 1);
    G8RTOS_InitSemaphore(&sem_4, 1);

    G8RTOS_InitFIFO(0);
    G8RTOS_InitFIFO(1);

    G8RTOS_AddThread(Obstacle_Thread, 252, "Obstacle", 2);
    G8RTOS_AddThread(Dino_Thread, 251, "Dino", 3);
    G8RTOS_AddThread(Reset_Thread, 250, "Reset", 1);

    //G8RTOS_AddThread(Read_JoystickPress_Thread, 252, "JoyRead", 3);

    //G8RTOS_Add_PeriodicEvent(Print_WorldCoords_P, 100, 1);
    G8RTOS_Add_PeriodicEvent(Get_Joystick_P, 10, 50);

    JOYSTICK_IntEnable();

    G8RTOS_Launch();

    while (true)
        ;
}

/************************************MAIN*******************************************/
