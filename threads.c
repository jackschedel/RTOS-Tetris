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
#define FRAME_X_OFF 30
#define FRAME_Y_OFF 10
#define BLOCK_SIZE 11
#define YELLOW 0x07FF
#define LIGHT_BLUE 0xFFE0
#define START_X 3
#define START_Y ROWS - 2

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

    uint8_t blockY = START_Y;

    uint8_t blockX = START_X;

    uint8_t curBlock = 5;

    uint8_t reRenderBlock = 1;

    ST7789_DrawLine(FRAME_X_OFF - 1, FRAME_Y_OFF - 1,
    FRAME_X_OFF + BLOCK_SIZE * COLS + 1,
                    FRAME_Y_OFF - 1, 0xFFFF);
    ST7789_DrawLine(FRAME_X_OFF - 1, FRAME_Y_OFF - 1,
    FRAME_X_OFF - 1,
                    FRAME_Y_OFF + BLOCK_SIZE * ROWS + 1, 0xFFFF);
    ST7789_DrawLine(FRAME_X_OFF - 1, FRAME_Y_OFF + BLOCK_SIZE * ROWS + 1,
    FRAME_X_OFF + BLOCK_SIZE * COLS + 1,
                    FRAME_Y_OFF + BLOCK_SIZE * ROWS + 1, 0xFFFF);
    ST7789_DrawLine(FRAME_X_OFF + BLOCK_SIZE * COLS + 1, FRAME_Y_OFF - 1,
    FRAME_X_OFF + BLOCK_SIZE * COLS + 1,
                    FRAME_Y_OFF + BLOCK_SIZE * ROWS + 1, 0xFFFF);

    // https://i.pinimg.com/736x/07/bf/d7/07bfd7e344183c428d841cf2813de97a.jpg
    // 1x4 is 5, 2x2 is 6

    // 3x3 grid (middle always filled)
    // 0 1 2
    // 3 F 4
    // 5 6 7
    unsigned char shapes[NUM_SHAPES - 2] = { 0b10011000, 0b00111000, 0b01110000, 0b01011000,
                                             0b11001000 };

    while (true)
    {
        // left = 1
        // right = 2
        // down = 3
        // rotate = 4
        uint8_t move = G8RTOS_ReadFIFO(0);

        if (move == 1)
        {
            if (blockX > 0)
            {
                blockX--;
            }
            else
            {
                move = 0;
            }
        }
        else
        {

            uint8_t dims = 0;

            if (curBlock <= 4)
            {
                dims = 0b00110010;
            }
            else if (curBlock == 5)
            {
                dims = 0b01000001;
            }
            else if (curBlock == 6)
            {
                dims = 0b00100010;
            }

            uint8_t curWidth = dims >> 4 * (blockRotation % 2) & 15;

            if (move == 2)
            {
                if (blockX + curWidth < COLS)
                {
                    blockX++;
                }
                else
                {
                    move = 0;
                }
            }
            else if (move == 3)
            {
                if (blockY > 1)
                {
                    blockY--;

                }
                else
                {
                    // hit bottom
                    move = -1;
                    curBlock++;
                    blockRotation = 1;
                    reRenderBlock = 1;
                    blockY = START_Y;
                    blockX = START_X;

                    if (curBlock >= NUM_SHAPES)
                        curBlock = 0;
                }
            }
            else if (move == 4)
            {
                if (curBlock != 6)
                {
                    reRenderBlock = 1;
                }
            }
        }

        // 2x2
        if (curBlock == 6)
        {
            if (reRenderBlock)
            {
                ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE * 2, BLOCK_SIZE * 2, YELLOW);
                reRenderBlock = 0;
            }
            else if (move == 1)
            {
                ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 2) * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE, BLOCK_SIZE * 2, 0);
                ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE, BLOCK_SIZE * 2, YELLOW);
            }
            else if (move == 2)
            {
                ST7789_DrawRectangle(FRAME_X_OFF + (blockX - 1) * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE, BLOCK_SIZE * 2, 0);
                ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE, BLOCK_SIZE * 2, YELLOW);
            }
            else if (move == 3)
            {
                ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                FRAME_Y_OFF + (blockY + 2) * BLOCK_SIZE,
                                     BLOCK_SIZE * 2, BLOCK_SIZE, 0);
                ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                FRAME_Y_OFF + blockY * BLOCK_SIZE,
                                     BLOCK_SIZE * 2, BLOCK_SIZE, YELLOW);
            }
        }
        // 1x4
        else if (curBlock == 5)
        {
            // straight up
            if (blockRotation % 2 - 1)
            {
                if (reRenderBlock || move != 3)
                {
                    if (move == 1)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE,
                        FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                             BLOCK_SIZE, BLOCK_SIZE * 4, 0);
                    }
                    else if (move == 2)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX - 1) * BLOCK_SIZE,
                        FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                             BLOCK_SIZE, BLOCK_SIZE * 4, 0);
                    }
                    else if (move == 4)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                        FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                             BLOCK_SIZE * 4, BLOCK_SIZE, 0);
                    }

                    ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE * 4, LIGHT_BLUE);
                    reRenderBlock = 0;
                }
                else if (move == 3)
                {
                    ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY + 3) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, 0);
                    ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, LIGHT_BLUE);
                }

            }
            else
            {
                if (reRenderBlock || move == 3)
                {
                    if (move == 3)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                        FRAME_Y_OFF + (blockY) * BLOCK_SIZE,
                                             BLOCK_SIZE * 4, BLOCK_SIZE, 0);
                    }
                    else if (move == 4)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                        FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                             BLOCK_SIZE, BLOCK_SIZE * 4, 0);
                    }

                    ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE * 4, BLOCK_SIZE, LIGHT_BLUE);
                    reRenderBlock = 0;
                }
                else if (move == 1)
                {
                    ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 4) * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, 0);
                    ST7789_DrawRectangle(FRAME_X_OFF + blockX * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, LIGHT_BLUE);
                }
                else if (move == 2)
                {
                    ST7789_DrawRectangle(FRAME_X_OFF + (blockX - 1) * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, 0);
                    ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 3) * BLOCK_SIZE,
                    FRAME_Y_OFF + (blockY - 1) * BLOCK_SIZE,
                                         BLOCK_SIZE, BLOCK_SIZE, LIGHT_BLUE);
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
            G8RTOS_WriteFIFO(0, 4);
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
