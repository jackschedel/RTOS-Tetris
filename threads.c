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
#define NUM_SHAPES 7

uint8_t point_count = 0;
uint8_t line_count = 1;
uint8_t blockRotation = 1;

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

    uint8_t blockY = ROWS + 4;

    uint8_t blockX = 4;

    uint8_t curBlock = 0;

    // (width << 4) | height

    // https://www.russlandjournal.de/en/wp-content/uploads/sites/2/2014/08/tetris-parts.jpg
    // w/ bottom 3 rotated 90 CCW
    unsigned char dims[NUM_SHAPES] = { 0b00010100, 0b00100011, 0b00100011, 0b00100010, 0b00100011,
                                       0b00100011, 0b00100011 };

    // 2x4 grid (top is only 1x4)
    // 0 4
    // 1 5
    // 2 6
    // 3 7
    unsigned char shapes[NUM_SHAPES] = { 0b11110000, 0b00010111, 0b01110001, 0b00110011, 0b01100011,
                                         0b01110010, 0b00110110 };

    while (true)
    {
        // left = 1
        // right = 2
        // down = 3
        uint8_t move = G8RTOS_ReadFIFO(0);

        if (move == 1)
        {
            if (blockX > 0)
            {
                blockX--;
            }
        }
        else
        {
            uint8_t curWidth = dims[curBlock] >> 4 * (1 % (blockRotation + 1)) & 15;
            uint8_t curHeight = dims[curBlock] >> 4 * (1 % blockRotation) & 15;

            if (move == 2)
            {
                if (blockX + curWidth < COLS)
                {
                    blockX++;
                }
            }
            else
            {
                if (blockY - curHeight >= 0)
                {
                    blockY--;

                }
                else
                {
                    // hit bottom
                    curBlock++;
                    blockRotation = 1;

                    if (curBlock >= NUM_SHAPES)
                        curBlock = 0;
                }
            }
        }

        UARTprintf("\nX: %d\n", blockX);
        UARTprintf("Y: %d\n", blockY);
        UARTprintf("Rotation: %d\n", blockRotation);

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

uint16_t joy_released = 1;
uint16_t rotate_released = 1;
uint16_t drop_released = 1;

void Get_Input_P()
{
    int16_t yRaw = JOYSTICK_GetY();

    int16_t xRaw = JOYSTICK_GetX();

    yRaw -= JOYSTICK_MIDPOINT;

    xRaw -= JOYSTICK_MIDPOINT;

    if (!joy_released && abs(yRaw) < JOYSTICK_DEADZONE && abs(xRaw) < JOYSTICK_DEADZONE)
    {
        joy_released = 1;
    }
    else if (joy_released && xRaw > JOYSTICK_DEADZONE)
    {
        G8RTOS_WriteFIFO(0, 1);
        joy_released = 0;
    }
    else if (joy_released && xRaw < -JOYSTICK_DEADZONE)
    {
        G8RTOS_WriteFIFO(0, 2);
        joy_released = 0;
    }
    else if (joy_released && yRaw < -JOYSTICK_DEADZONE)
    {
        G8RTOS_WriteFIFO(0, 3);
        joy_released = 0;
    }

    uint8_t buttons = MultimodButtons_Get();

    if (buttons & 2)
    {
        if (rotate_released)
        {
            blockRotation++;
            if (blockRotation > 4)
                blockRotation = 1;
            rotate_released = false;
        }
    }
    else
    {
        rotate_released = true;
    }

    if (buttons & 4)
    {
        if (drop_released)
        {
            // todo drop
            drop_released = false;
        }
    }
    else
    {
        drop_released = true;
    }

    G8RTOS_Yield();

}

void Gravity_P()
{
    G8RTOS_WriteFIFO(0, 3);
    G8RTOS_Yield();
}

void setStaticBlockBit(int row, int col, int value)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        abort();
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    if (value)
        static_blocks[byteIndex] |= (1 << bitInByte);
    else
        static_blocks[byteIndex] &= ~(1 << bitInByte);
}

uint8_t getStaticBlockBit(int row, int col)
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
