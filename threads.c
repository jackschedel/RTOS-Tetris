// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"
#include "./MultimodDrivers/multimod.h"
#include <driverlib/uart.h>
#include <driverlib/interrupt.h>
#include <driverlib/uartstdio.h>
#include "./G8RTOS/G8RTOS_Scheduler.h"
#include "./G8RTOS/G8RTOS_IPC.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MiscFunctions/LinAlg/inc/linalg.h"
#include "MiscFunctions/LinAlg/inc/quaternions.h"
#include "MiscFunctions/LinAlg/inc/vect3d.h"
#include "MiscFunctions/Shapes/inc/cube.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

//#define delay_0_1_s     (1600000/3)
#define thread_delay     (1600000/16)

#define JUMP_HEIGHT 68
#define JUMP_SLOWDOWN 1
#define LINE_HEIGHT 35
#define JOYSTICK_SENS 2850
#define DINO_X 10
#define OBSTACLE_COUNT 6
#define OBSTACLE_RESET_DISTANCE 500
#define OBSTACLE_SLOWDOWN 3

// only ever writable by Dino_Thread
int8_t dino_pos = 0;

// needs to be reset
uint8_t past_peak = 1;

uint8_t score = 0;

/*************************************Defines***************************************/

/********************************Public Functions***********************************/

void EmptyThreadWait()
{
    while (1)
        ;
}

void Idle_Thread()
{
    while (1)
        ;
}

void Obstacle_Thread()
{
    // green:
    // 0b11110000000

    uint8_t i;
    int16_t obstacles_x[OBSTACLE_COUNT] = { 300, 500, 580, 700, 920, 1100 };

    while (true)
    {

        for (i = 0; i < OBSTACLE_COUNT; i++)
        {
            //clear
            if (obstacles_x[i] >= 0 && obstacles_x[i] < X_MAX - 2)
            {
                ST7789_DrawRectangle(obstacles_x[i], LINE_HEIGHT + 1, 1, 10, 0);
            }

            obstacles_x[i]--;

            if (obstacles_x[i] >= 0 && obstacles_x[i] < X_MAX - 2)
            {
                ST7789_DrawRectangle(obstacles_x[i] - 1, LINE_HEIGHT + 1, 1, 10, 0b11110000000);
            }

            if (obstacles_x[i] < -OBSTACLE_RESET_DISTANCE)
            {
                obstacles_x[i] = OBSTACLE_RESET_DISTANCE;
            }

            if (obstacles_x[i] >= DINO_X && obstacles_x[i] <= DINO_X + 4)
            {
                if (dino_pos < 10)
                {
                    obstacles_x[0] = 300;
                    obstacles_x[1] = 500;

                    obstacles_x[2] = 580;

                    obstacles_x[3] = 700;
                    obstacles_x[4] = 920;
                    obstacles_x[5] = 1100;

                    past_peak = 1;

                    G8RTOS_SignalSemaphore(&sem_died);
                }
                else if (obstacles_x[i] == DINO_X + 4)
                {
                    score++;
                }
            }
        }

        G8RTOS_Sleep(OBSTACLE_SLOWDOWN);

    }
}

void Dino_Thread()
{
    while (true)
    {

        if (dino_pos <= 0 && past_peak)
        {
            dino_pos = 0;
            past_peak = 0;
            G8RTOS_WaitSemaphore(&sem_jumping);
        }

        //clear

        if (!past_peak)
        {
            ST7789_DrawRectangle(DINO_X, LINE_HEIGHT + 1 + dino_pos, 4, 1, 0);
            dino_pos++;
            ST7789_DrawRectangle(DINO_X, LINE_HEIGHT + 10 + dino_pos, 4, 1, 0xFFFF);

        }
        else
        {
            ST7789_DrawRectangle(DINO_X, LINE_HEIGHT + 10 + dino_pos, 4, 1, 0);
            dino_pos--;
            ST7789_DrawRectangle(DINO_X, LINE_HEIGHT + 1 + dino_pos, 4, 1, 0xFFFF);

        }

        if (dino_pos >= JUMP_HEIGHT)
        {
            past_peak = 1;
        }

        G8RTOS_Sleep(JUMP_SLOWDOWN);
    }

}

uint8_t is_dead = 0;

void Reset_Thread()
{
    while (true)
    {
        ST7789_Fill(0);
        ST7789_DrawRectangle(DINO_X, LINE_HEIGHT + 1, 4, 10, 0xFFFF);
        ST7789_DrawLine(0, LINE_HEIGHT, 240, LINE_HEIGHT, 0xFFFF);
        is_dead = 0;

        G8RTOS_WaitSemaphore(&sem_died);
        is_dead = 1;
        ST7789_Fill(0xFF);
        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("\nScore: %d\n", score);

        G8RTOS_SignalSemaphore(&sem_UART);
        score = 0;

        if (dino_pos == 0)
            G8RTOS_SignalSemaphore(&sem_jumping);

        while (!MultimodButtons_Get())
        {
            // doing this rather than wait semaphore to prevent context switching

        }

    }
}

// unused
void Read_Buttons_Thread()
{

    while (true)
    {
        G8RTOS_WaitSemaphore(&sem_buttons);
        uint8_t buttons = MultimodButtons_Get();

        if (buttons)
        {
            if (is_dead)
                G8RTOS_SignalSemaphore(&sem_died);

            G8RTOS_Sleep(200);
            IntEnable(INT_GPIOE);
        }
    }

}

uint16_t released = 1;

void Get_Joystick_P()
{
    uint16_t raw = JOYSTICK_GetY();

    if (raw > JOYSTICK_SENS)
    {
        if (sem_jumping < 0 && !is_dead && released)
        {
            G8RTOS_SignalSemaphore(&sem_jumping);
            released = 0;
        }
    }
    else
    {
        released = 1;
    }

    G8RTOS_Yield();

}

// unused
void GPIOE_Handler()
{
    G8RTOS_SignalSemaphore(&sem_buttons);
    GPIOIntClear(GPIO_PORTE_BASE, 0);
    IntDisable(INT_GPIOE);
    IntPendClear(INT_GPIOE);
}

void Event3()
{

}

void Event4()
{

}

/********************************Public Functions***********************************/
