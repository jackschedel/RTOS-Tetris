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

#define ROWS 20
#define COLS 10
#define BLOCKS_ARRAY_SIZE ((ROWS * COLS + 7) / 8)
#define BITS_PER_BYTE 8
#define JOYSTICK_MIDPOINT 2100
#define JOYSTICK_DEADZONE 200

uint8_t point_count = 0;
uint8_t line_count = 1;

unsigned char static_blocks[BLOCKS_ARRAY_SIZE] = { 0 };

/*************************************Defines***************************************/

/********************************Public Functions***********************************/

void Idle_Thread()
{
    while (1)
        ;
}

void FallingBlock_Thread()
{

    while (true)
    {

    }
}

void StaticBlocks_Thread()
{
    while (true)
    {

    }

}

uint8_t is_dead = 0;

void DrawUI_Thread()
{
    while (true)
    {
        ST7789_Fill(0);

    }
}

uint16_t released = 1;

void Get_Joystick_P()
{
    int16_t yRaw = JOYSTICK_GetY();

    int16_t xRaw = JOYSTICK_GetX();

    yRaw -= JOYSTICK_MIDPOINT;

    xRaw -= JOYSTICK_MIDPOINT;

    // left = 1
    // right = 2
    // down = 3
    if (!released && abs(yRaw) < JOYSTICK_DEADZONE && abs(xRaw) < JOYSTICK_DEADZONE)
    {
        released = 1;
    }
    else if (released && xRaw > JOYSTICK_DEADZONE)
    {
        UARTprintf("\nLeft\n", xRaw);
        G8RTOS_WriteFIFO(0, 1);
        released = 0;
    }
    else if (released && xRaw < -JOYSTICK_DEADZONE)
    {
        UARTprintf("\nRight\n", xRaw);
        G8RTOS_WriteFIFO(0, 2);
        released = 0;
    }
    else if (released && yRaw < -JOYSTICK_DEADZONE)
    {
        UARTprintf("\nDown\n", xRaw);
        G8RTOS_WriteFIFO(0, 3);
        released = 0;
    }

    G8RTOS_Yield();

}

void FallingBlockGravity_P()
{

    G8RTOS_Yield();

}

void GPIOE_Handler()
{
    //G8RTOS_SignalSemaphore(&sem_buttons);
    GPIOIntClear(GPIO_PORTE_BASE, 0);
    IntDisable(INT_GPIOE);
    IntPendClear(INT_GPIOE);
}

void setBit(int row, int col, int value)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        abort();
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    if (value)
        static_blocks[byteIndex] |= (1 << bitInByte); // Set bit
    else
        static_blocks[byteIndex] &= ~(1 << bitInByte); // Clear bit
}

uint8_t getBit(int row, int col)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        abort();
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    return (static_blocks[byteIndex] >> bitInByte) & 1;
}

/********************************Public Functions***********************************/
