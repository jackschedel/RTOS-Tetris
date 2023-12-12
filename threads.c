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
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include "./G8RTOS/G8RTOS_Scheduler.h"
#include "./G8RTOS/G8RTOS_IPC.h"
#include "./G8RTOS/G8RTOS_CriticalSection.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MiscFunctions/LinAlg/inc/linalg.h"
#include "MiscFunctions/LinAlg/inc/quaternions.h"
#include "MiscFunctions/LinAlg/inc/vect3d.h"
#include "MiscFunctions/Shapes/inc/cube.h"
#include "./MultimodDrivers/fontlibrary.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

//#define delay_0_1_s     (1600000/3)
#define thread_delay     (1600000/16)

#define ROWS 20
#define COLS 10
#define BLOCKS_ARRAY_SIZE ((ROWS * COLS + 7) / 8)
#define BITS_PER_BYTE 8
#define JOYSTICK_MIDPOINT 2100
#define JOYSTICK_DEADZONE 500
#define NUM_SHAPES 7
#define FRAME_X_OFF 60
#define FRAME_Y_OFF 83
#define BLOCK_SIZE 10
#define DARK_GRAY 0x31A6
#define GRAY 0xAD55
#define SHADOW_COLOR colors[curBlock]

#define START_X 3
#define START_Y ROWS - 2

#define MOVE_NONE 0
#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define MOVE_DOWN 3
#define MOVE_ROTATE 4
#define MOVE_INSTADROP 5
#define MOVE_SWAP 6

#define FONT_WIDTH 8

#define STRAIGHT_UP blockRotation % 2 - 1
#define HORIZONTAL 1

#define SQUARE 5
#define LINE 6

#define GRAVITY_THREAD_ID 50
#define START_SPEED 700.0

#define ROT_VERTICAL (blockRotation - 1) % 2

// maximum value is NUM_SHAPES
#define PREVIEW_COUNT 4

uint8_t point_count = 0;
uint8_t highscore = 0;
uint8_t line_count = 1;
uint8_t blockRotation = 1;
int8_t blockY = START_Y;
int8_t blockX = START_X;
uint8_t resetting = 0;
uint8_t timer = 0;
uint32_t score = 0;
uint8_t lines_cleared = 0;
uint8_t level_num = 1;
uint8_t curBlockInd = 0;
int8_t heldBlock = -1;
uint8_t curBlock;
int8_t shadowX[4] = { -1 };
int8_t shadowY[4] = { -1 };

unsigned char static_blocks[BLOCKS_ARRAY_SIZE] = { 0 };
unsigned char old_static_blocks[BLOCKS_ARRAY_SIZE] = { 0 };
char piece_grab_bag[NUM_SHAPES * 2] = { 255 };

/*************************************Defines***************************************/

/********************************Public Functions***********************************/

void Idle_Thread()
{
    while (1)
        ;
}

void Lost_Thread()
{
    while (true)
    {
        G8RTOS_WaitSemaphore(&sem_lost);

        G8RTOS_Sleep(500);

        for (uint8_t i = 0; i < BLOCKS_ARRAY_SIZE; i++)
        {
            static_blocks[i] = 0;
        }

        UARTprintf("Score: %d\n", score);
        if (score > highscore)
        {
            highscore = score;
        }
        G8RTOS_SignalSemaphore(&sem_update_ui);
        G8RTOS_Yield();

        G8RTOS_Change_Period(GRAVITY_THREAD_ID, (uint32_t) START_SPEED);

        ST7789_DrawRectangle(FRAME_X_OFF, FRAME_Y_OFF,
        BLOCK_SIZE * COLS - 1,
                             BLOCK_SIZE * ROWS - 1, 0);
        score = 0;
        lines_cleared = 0;
        level_num = 1;

        blockY = START_Y;
        blockX = START_X;

        renderCrosshatchGrid();

        randomiseGrabBag();
        curBlockInd = 0;
        curBlock = piece_grab_bag[curBlockInd];
        heldBlock = -1;

        G8RTOS_WriteFIFO(0, 0);
        resetting = 0;
        G8RTOS_SignalSemaphore(&sem_update_ui);

        ST7789_DrawRectangle(FRAME_X_OFF - (5 * FONT_WIDTH) - 2 - FONT_WIDTH,
        FRAME_Y_OFF + 9 * BLOCK_SIZE - 2,
                             6 * FONT_WIDTH, FONT_WIDTH, 0);

        ST7789_DrawRectangle(FRAME_X_OFF - (5 * FONT_WIDTH) - 2 - FONT_WIDTH,
        FRAME_Y_OFF + 5 * BLOCK_SIZE - 2,
                             6 * FONT_WIDTH, FONT_WIDTH, 0);

        ST7789_DrawRectangle(FRAME_X_OFF - (5 * FONT_WIDTH) - 2 - FONT_WIDTH,
        FRAME_Y_OFF + 1 * BLOCK_SIZE - 2,
                             6 * FONT_WIDTH, FONT_WIDTH, 0);

    }
}

void FallingBlock_Thread()
{
    uint8_t moveValid = 1;

    uint8_t piecePlaced = 0;
    uint8_t instaDrop = 0;

    int8_t wallKick = 0;
    uint8_t curr, blockAtPos = 0;

    uint16_t colors[NUM_SHAPES] = { 0xF800, 0x055F, 0x07E0, 0xF81F, 0x001F, 0x07FF, 0xFFE0 };

    ST7789_DrawOutline(FRAME_X_OFF - 1, FRAME_Y_OFF - 1, BLOCK_SIZE * COLS + 2,
    BLOCK_SIZE * ROWS + 2,
                       0xFFFF);

    renderCrosshatchGrid();

    randomiseGrabBag();

    curBlock = piece_grab_bag[curBlockInd];

    // https://i.pinimg.com/736x/07/bf/d7/07bfd7e344183c428d841cf2813de97a.jpg
    // 1x4 is 5, 2x2 is 6

    // 3x3 grid (middle always filled)
    // 0 1 2
    // 3 F 4
    // 5 6 7
    unsigned char shapes[4][NUM_SHAPES] = { { 0b10011000, 0b00111000, 0b01110000, 0b01011000,
                                              0b11001000, 0b00010110, 0b00011000 },
                                            { 0b01100010, 0b01000011, 0b01001001, 0b01001010,
                                              0b00101010, 0b00010110, 0b01000010 },
                                            { 0b00011001, 0b00011100, 0b00001110, 0b00011010,
                                              0b00010011, 0b00010110, 0b00011000 },
                                            { 0b01000110, 0b11000010, 0b10010010, 0b01010010,
                                              0b01010100, 0b00010110, 0b01000010 } };

    G8RTOS_Add_PeriodicEvent(Gravity_P, (uint32_t) START_SPEED, 50, GRAVITY_THREAD_ID);

    if (level_num != 1)
    {
        float_t period = START_SPEED * pow((0.9), level_num) + (START_SPEED / 20.0);
        G8RTOS_Change_Period(GRAVITY_THREAD_ID, (uint32_t) period);
    }

    G8RTOS_WriteFIFO(0, 0);

    while (true)
    {
        // left = 1
        // right = 2
        // down = 3
        // rotate = 4
        uint8_t move = G8RTOS_ReadFIFO(0);

        if (resetting)
            continue;

        instaDrop = 0;
        moveValid = 0;
        wallKick = 0;

        // clear shadow
        if (move != MOVE_DOWN && shadowX[0] != -1)
        {
            for (int8_t j = 0; j < 4; j++)
            {
                ST7789_DrawRectangle(
                FRAME_X_OFF + shadowX[j] * BLOCK_SIZE + 1,
                                     FRAME_Y_OFF + shadowY[j] * BLOCK_SIZE + 1,
                                     BLOCK_SIZE - 2,
                                     BLOCK_SIZE - 2, 0);
            }
        }

        if (move == MOVE_NONE)
        {
            // rendering for held piece and lookahead preview
            int8_t yOffset = -1;
            int8_t xOffset = -1;
            uint8_t pieceOffset = 0;
            uint8_t previewBlock;

            for (int8_t k = 0; k <= PREVIEW_COUNT; k++)
            {
                // when k == 0, we are rendering the held piece
                if (k == 0)
                {
                    previewBlock = heldBlock;
                    xOffset = -(COLS + 5);
                }
                else
                {
                    pieceOffset++;
                    previewBlock = piece_grab_bag[(curBlockInd + pieceOffset) % NUM_SHAPES];
                }
                yOffset += 5;

                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (!resetting)
                        {
                            if (i == 1 && j == 1)
                            {
                                blockAtPos = 1;
                            }
                            else
                            {
                                blockAtPos = shapes[1][previewBlock]
                                        >> (previewBlock == SQUARE ? curr : 7 - curr) & 1;
                                curr++;
                            }

                            if (blockAtPos)
                            {
                                ST7789_DrawRectangle(
                                        FRAME_X_OFF + (COLS + 1 + i + xOffset) * BLOCK_SIZE + 1
                                                + (previewBlock == LINE ? BLOCK_SIZE / 2 : 0),
                                        FRAME_Y_OFF + (ROWS - yOffset + j) * BLOCK_SIZE + 1,
                                        BLOCK_SIZE - 2,
                                        BLOCK_SIZE - 2, colors[previewBlock]);
                            }
                            else
                            {
                                ST7789_DrawRectangle(
                                        FRAME_X_OFF + (COLS + 1 + i + xOffset) * BLOCK_SIZE + 1
                                                + ((previewBlock == LINE && i == 2) ?
                                                        (BLOCK_SIZE / 2) - 2 : 0),
                                        FRAME_Y_OFF + (ROWS - yOffset + j) * BLOCK_SIZE + 1,
                                        BLOCK_SIZE + ((previewBlock == LINE) ? BLOCK_SIZE / 2 : 0),
                                        BLOCK_SIZE, 0);
                            }
                        }
                    }
                }
                if (previewBlock == LINE)
                {
                    ST7789_DrawRectangle(
                    FRAME_X_OFF + (COLS + 2 + xOffset) * BLOCK_SIZE + 1 + BLOCK_SIZE / 2,
                                         FRAME_Y_OFF + (ROWS - yOffset + 3) * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2,
                                         BLOCK_SIZE - 2, colors[previewBlock]);
                }
                else
                {
                    ST7789_DrawRectangle(
                    FRAME_X_OFF + (COLS + 2 + xOffset) * BLOCK_SIZE + 1 + BLOCK_SIZE / 2,
                                         FRAME_Y_OFF + (ROWS - yOffset + 3) * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2,
                                         BLOCK_SIZE - 2, 0);
                    ST7789_DrawRectangle(
                    FRAME_X_OFF + (COLS + 3 + xOffset) * BLOCK_SIZE + -1,
                                         FRAME_Y_OFF + (ROWS - yOffset) * BLOCK_SIZE + 1, 2,
                                         BLOCK_SIZE * 4,
                                         0);
                }
                if (k == 0)
                {
                    yOffset = -1;
                    xOffset = -1;
                }
            }

            // piece offset override on spawn
            if (curBlock <= 4)
            {
                blockY--;
            }
            else if (curBlock == 5)
            {
                blockX++;
            }
            else if (curBlock == 6)
            {
                // 1x4 spawn adjust if second-to-last is filled
                if (!getStaticBlockBit(blockX, blockY) && !getStaticBlockBit(blockX + 1, blockY)
                        && !getStaticBlockBit(blockX + 2, blockY)
                        && !getStaticBlockBit(blockX + 3, blockY))
                {
                    blockY--;

                }

                moveValid = 1;
            }
        }
        else if (move == MOVE_INSTADROP)
        {
            move = MOVE_DOWN;
            instaDrop = 1;
            score += 2;
        }

        moveValid = 1;

        if (move == MOVE_LEFT)
        {
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {
                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos && getStaticBlockBit(blockX + i - 1, blockY + j))
                    {
                        moveValid = 0;
                    }
                }
            }

            if (curBlock == LINE && ROT_VERTICAL)
            {
                if (getStaticBlockBit(blockX, blockY + 3))
                {
                    moveValid = 0;
                }
            }

        }
        else if (move == MOVE_RIGHT)
        {
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {
                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos && getStaticBlockBit(blockX + i + 1, blockY + j))
                    {
                        moveValid = 0;
                    }
                }
            }

            if (curBlock == LINE)
            {
                if (ROT_VERTICAL)
                {
                    if (getStaticBlockBit(blockX + 2, blockY + 3))
                    {
                        moveValid = 0;
                    }
                }
                else
                {
                    if (getStaticBlockBit(blockX + 4, blockY + 1))
                    {
                        moveValid = 0;
                    }
                }
            }
        }

        else if (move == MOVE_DOWN)
        {
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {
                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos && getStaticBlockBit(blockX + i, blockY + j - 1))
                    {
                        moveValid = 0;
                    }
                }
            }

            if (curBlock == LINE)
            {
                if (blockRotation % 2)
                {
                    if (getStaticBlockBit(blockX + 3, blockY))
                    {
                        moveValid = 0;
                    }
                }
            }

            if (!moveValid)
            {
                // place static piece if hit
                piecePlaced = 1;
                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (i == 1 && j == 1)
                        {
                            blockAtPos = 1;
                        }
                        else
                        {
                            blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr)
                                    & 1;
                            curr++;
                        }

                        if (blockAtPos)
                        {
                            ST7789_DrawRectangle(
                            FRAME_X_OFF + (blockX + i) * BLOCK_SIZE + 1,
                                                 FRAME_Y_OFF + (blockY + j) * BLOCK_SIZE + 1,
                                                 BLOCK_SIZE - 2,
                                                 BLOCK_SIZE - 2,
                                                 GRAY);
                            setStaticBlockBit(blockX + i, blockY + j, 1, 1);
                        }
                    }
                }

                if (curBlock == LINE)
                {
                    if (ROT_VERTICAL)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE + 1,
                        FRAME_Y_OFF + (blockY + 3) * BLOCK_SIZE + 1,
                                             BLOCK_SIZE - 2, BLOCK_SIZE - 2, GRAY);
                        setStaticBlockBit(blockX + 1, blockY + 3, 1, 1);
                    }
                    else
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 3) * BLOCK_SIZE + 1,
                        FRAME_Y_OFF + (blockY + 1) * BLOCK_SIZE + 1,
                                             BLOCK_SIZE - 2, BLOCK_SIZE - 2, GRAY);
                        setStaticBlockBit(blockX + 3, blockY + 1, 1, 1);
                    }
                }
            }
        }
        else if (move == MOVE_ROTATE)
        {
            // line rotation offset for proper SRS-compliant rotation
            if (curBlock == LINE)
            {
                if (blockRotation == 1)
                {
                    blockX++;
                    blockY--;
                }
                else if (blockRotation == 2)
                {
                    blockX--;
                }
                else if (blockRotation == 4)
                {
                    blockY++;
                }
            }

            blockRotation++;
            if (blockRotation > 4)
                blockRotation = 1;

            // check if rotation would hit anything
            moveValid = 1;
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {
                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos)
                    {
                        if (getStaticBlockBit(blockX + i, blockY + j))
                        {
                            moveValid = 0;
                        }
                    }
                }
            }
            if (curBlock == LINE)
            {
                if (ROT_VERTICAL)
                {
                    if (getStaticBlockBit(blockX + 1, blockY + 3))
                    {
                        moveValid = 0;
                    }
                }
                else
                {
                    if (getStaticBlockBit(blockX + 3, blockY + 1))
                    {
                        moveValid = 0;
                    }
                }
            }

            // if rotation failed, try wallkick #1: try moving block left 1 unit, then rotating
            if (!moveValid)
            {
                moveValid = 1;
                blockX--;
                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (i == 1 && j == 1)
                        {
                            blockAtPos = 1;
                        }
                        else
                        {
                            blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr)
                                    & 1;
                            curr++;
                        }

                        if (blockAtPos)
                        {
                            if (getStaticBlockBit(blockX + i, blockY + j))
                            {
                                moveValid = 0;
                            }
                        }
                    }
                }
                if (curBlock == LINE)
                {
                    if (ROT_VERTICAL)
                    {
                        if (getStaticBlockBit(blockX + 1, blockY + 3))
                        {
                            moveValid = 0;
                        }
                    }
                    else
                    {
                        if (getStaticBlockBit(blockX + 3, blockY + 1))
                        {
                            moveValid = 0;
                        }
                    }
                }

                if (moveValid)
                {
                    wallKick = -1;
                }
                blockX++;
            }

            // if wallkick try #1 failed, try wallkick #2: try moving block right 1 unit (from original), then rotating
            if (!moveValid)
            {
                moveValid = 1;
                blockX++;
                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (i == 1 && j == 1)
                        {
                            blockAtPos = 1;
                        }
                        else
                        {
                            blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr)
                                    & 1;
                            curr++;
                        }

                        if (blockAtPos)
                        {
                            if (getStaticBlockBit(blockX + i, blockY + j))
                            {
                                moveValid = 0;
                            }
                        }
                    }
                }
                if (curBlock == LINE)
                {
                    if (ROT_VERTICAL)
                    {
                        if (getStaticBlockBit(blockX + 1, blockY + 3))
                        {
                            moveValid = 0;
                        }
                    }
                    else
                    {
                        if (getStaticBlockBit(blockX + 3, blockY + 1))
                        {
                            moveValid = 0;
                        }
                    }
                }

                if (moveValid)
                {
                    wallKick = 1;
                }
                blockX--;
            }

            // undo the rotation so we can clear it later
            blockRotation--;
            if (blockRotation == 0)
            {
                blockRotation = 4;
            }
            if (curBlock == LINE)
            {
                if (blockRotation == 1)
                {
                    blockX--;
                    blockY++;
                }
                else if (blockRotation == 2)
                {
                    blockX++;
                }
                else if (blockRotation == 4)
                {
                    blockY--;
                }
            }

        }

        // all rendering of non-static block done here
        if (moveValid)
        {
            // clear previous block
            if (move != MOVE_NONE)
            {
                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (i == 1 && j == 1)
                        {
                            blockAtPos = 1;
                        }
                        else
                        {
                            blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr)
                                    & 1;
                            curr++;
                        }

                        if (blockAtPos)
                        {
                            ST7789_DrawRectangle(
                            FRAME_X_OFF + (blockX + i) * BLOCK_SIZE + 1,
                                                 FRAME_Y_OFF + (blockY + j) * BLOCK_SIZE + 1,
                                                 BLOCK_SIZE - 2,
                                                 BLOCK_SIZE - 2, 0);
                        }
                    }
                }
                if (curBlock == LINE)
                {
                    if (ROT_VERTICAL)
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE + 1,
                        FRAME_Y_OFF + (blockY + 3) * BLOCK_SIZE + 1,
                                             BLOCK_SIZE - 2, BLOCK_SIZE - 2, 0);
                    }
                    else
                    {
                        ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 3) * BLOCK_SIZE + 1,
                        FRAME_Y_OFF + (blockY + 1) * BLOCK_SIZE + 1,
                                             BLOCK_SIZE - 2, BLOCK_SIZE - 2, 0);
                    }
                }
            }

            // do piece transformation
            if (move == MOVE_LEFT)
            {
                blockX--;
            }
            else if (move == MOVE_RIGHT)
            {
                blockX++;
            }
            else if (move == MOVE_DOWN)
            {
                blockY--;
            }
            else if (move == MOVE_ROTATE)
            {
                blockX += wallKick;

                if (curBlock == LINE)
                {
                    if (blockRotation == 1)
                    {
                        blockX++;
                        blockY--;
                    }
                    else if (blockRotation == 2)
                    {
                        blockX--;
                    }
                    else if (blockRotation == 4)
                    {
                        blockY++;
                    }
                }

                blockRotation++;
                if (blockRotation > 4)
                    blockRotation = 1;
            }
            else if (move == MOVE_SWAP)
            {
                if (heldBlock == -1)
                {
                    heldBlock = piece_grab_bag[++curBlockInd];
                }
                uint8_t tempSwap = curBlock;
                curBlock = heldBlock;
                heldBlock = tempSwap;

                blockRotation = 1;
                blockY = START_Y;
                blockX = START_X;

            }
        }

        if (move != MOVE_SWAP && !piecePlaced)
        {
            // if spawning a new piece, check if the player lost
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {
                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos)
                    {
                        if (move == MOVE_NONE && getStaticBlockBit(blockX + i, blockY + j))
                        {
                            resetting = 1;
                        }
                    }
                }
            }
            if (curBlock == LINE)
            {
                if (ROT_VERTICAL)
                {
                    if (move == MOVE_NONE && getStaticBlockBit(blockX + 1, blockY + 3))
                    {
                        resetting = 1;
                    }
                }
                else
                {
                    if (move == MOVE_NONE && getStaticBlockBit(blockX + 3, blockY + 1))
                    {
                        resetting = 1;
                    }
                }
            }

            if (resetting)
            {
                G8RTOS_SignalSemaphore(&sem_lost);
                continue;
            }

            // render new block
            curr = 0;
            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {

                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos)
                    {
                        ST7789_DrawRectangle(
                        FRAME_X_OFF + (blockX + i) * BLOCK_SIZE + 1,
                                             FRAME_Y_OFF + (blockY + j) * BLOCK_SIZE + 1,
                                             BLOCK_SIZE - 2,
                                             BLOCK_SIZE - 2, colors[curBlock]);
                    }
                }
            }
            if (curBlock == LINE)
            {
                if (ROT_VERTICAL)
                {
                    ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + (blockY + 3) * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, colors[curBlock]);
                }
                else
                {
                    ST7789_DrawRectangle(FRAME_X_OFF + (blockX + 3) * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + (blockY + 1) * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, colors[curBlock]);
                }

            }
        }

        // piece placed logic
        if (piecePlaced)
        {
            if (!resetting)
            {
                // spawn new block eventually (once StaticBlocks is done)
                G8RTOS_WriteFIFO(0, MOVE_NONE);

                // check for line clear
                G8RTOS_SignalSemaphore(&sem_clearLine);
                G8RTOS_Yield();
            }

            curBlockInd++;
            if (curBlockInd >= NUM_SHAPES)
            {
                curBlockInd = 0;
            }

            curBlock = piece_grab_bag[curBlockInd];

            // re-random 1 early to allow lookahead on edge
            if (curBlockInd == NUM_SHAPES - 1)
            {
                randomiseGrabBag();
            }

            // reset coords for newly spawned block
            shadowX[0] = -1;
            blockRotation = 1;
            blockY = START_Y;
            blockX = START_X;

            piecePlaced = 0;

        }
        else if (instaDrop)
        {
            // chain trigger another instadrop if the piece hasn't been placed yet
            G8RTOS_WriteFIFO(0, 5);
        }
        else
        {
            // find y offset for shadow
            uint8_t previewOffset = 1;
            uint8_t done = 0;
            while (!done)
            {
                curr = 0;
                for (int8_t j = 2; j >= 0; j--)
                {
                    for (int8_t i = 0; i < 3; i++)
                    {
                        if (i == 1 && j == 1)
                        {
                            blockAtPos = 1;
                        }
                        else
                        {
                            blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr)
                                    & 1;
                            curr++;
                        }

                        if (blockAtPos)
                        {
                            if (getStaticBlockBit(blockX + i, blockY + j - previewOffset))
                            {
                                done = 1;

                            }
                        }
                    }
                }
                if (curBlock == LINE)
                {
                    if (ROT_VERTICAL)
                    {
                        if (getStaticBlockBit(blockX + 1, blockY + 3 - previewOffset))
                        {
                            done = 1;
                        }
                    }
                    else
                    {
                        if (getStaticBlockBit(blockX + 3, blockY + 1 - previewOffset))
                        {
                            done = 1;
                        }
                    }
                }
                previewOffset++;
            }
            previewOffset -= 2;

            // render shadow
            uint8_t shadowIndex = 0;
            curr = 0;

            for (int8_t j = 2; j >= 0; j--)
            {
                for (int8_t i = 0; i < 3; i++)
                {

                    if (i == 1 && j == 1)
                    {
                        blockAtPos = 1;
                    }
                    else
                    {
                        blockAtPos = shapes[(blockRotation - 1) % 4][curBlock] >> (7 - curr) & 1;
                        curr++;
                    }

                    if (blockAtPos)
                    {
                        shadowX[shadowIndex] = blockX + i;
                        shadowY[shadowIndex++] = blockY + j - previewOffset;
                        ST7789_DrawOutline(
                                FRAME_X_OFF + (blockX + i) * BLOCK_SIZE + 1,
                                FRAME_Y_OFF + (blockY + j - previewOffset) * BLOCK_SIZE + 1,
                                BLOCK_SIZE - 2,
                                BLOCK_SIZE - 2,
                                SHADOW_COLOR);
                    }

                }
            }
            if (curBlock == LINE)
            {
                if (ROT_VERTICAL)
                {

                    ST7789_DrawOutline(FRAME_X_OFF + (blockX + 1) * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + (blockY + 3 - previewOffset) * BLOCK_SIZE + 1,
                                       BLOCK_SIZE - 2, BLOCK_SIZE - 2, SHADOW_COLOR);
                    shadowX[shadowIndex] = blockX + 1;
                    shadowY[shadowIndex] = blockY + 3 - previewOffset;

                }
                else
                {
                    ST7789_DrawOutline(FRAME_X_OFF + (blockX + 3) * BLOCK_SIZE + 1,
                            FRAME_Y_OFF + (blockY + 1 - previewOffset) * BLOCK_SIZE + 1,
                            BLOCK_SIZE - 2, BLOCK_SIZE - 2, SHADOW_COLOR);
                    shadowX[shadowIndex] = blockX + 3;
                    shadowY[shadowIndex] = blockY + 1 - previewOffset;
                }
            }
        }

        G8RTOS_Yield();
    }
}

void StaticBlocks_Thread()
{
    int8_t i, j, numCleared, prevNumCleared = 0;
    uint8_t old, shifted, prevLevelNum;

    while (true)
    {
        G8RTOS_WaitSemaphore(&sem_clearLine);

        prevLevelNum = level_num;
        prevNumCleared = numCleared;

        // check the number of lines cleared in this drop
        numCleared = 0;
        for (i = blockY + 3; i >= blockY; i--)
        {
            if (i >= ROWS)
                break;

            if (staticCheckClear(i))
            {
                if (!numCleared)
                {
                    for (j = 0; j < BLOCKS_ARRAY_SIZE; j++)
                    {
                        old_static_blocks[j] = static_blocks[j];
                    }
                }
                numCleared++;
                lines_cleared++;
                slideStaticBlocks(i);
            }
        }

        G8RTOS_SignalSemaphore(&sem_update_ui);

        if (!numCleared)
            continue;

        level_num = (lines_cleared / 10) + 1;

        if (prevLevelNum != level_num)
        {
            // https://www.desmos.com/calculator/ir1s0pmyo5
            float_t period = START_SPEED * pow((0.9), level_num) + (START_SPEED / 20.0);
            G8RTOS_Change_Period(GRAVITY_THREAD_ID, (uint32_t) period);
        }

        // update score
        if (numCleared == 1)
        {
            score += 100 * level_num;
        }
        else if (numCleared == 2)
        {
            score += 300 * level_num;
        }
        else if (numCleared == 3)
        {
            score += 500 * level_num;
        }
        else if (numCleared == 4)
        {
            if (prevNumCleared == 4)
            {
                score += 1200 * level_num;
            }
            else
            {
                score += 800 * level_num;
            }
        }

        // shift down the static blocks
        for (++i; i < ROWS - numCleared; i++)
        {
            for (j = 0; j < COLS; j++)
            {
                // shift staticBlocksArr
                old = getOldStaticBlockBit(j, i);
                shifted = getStaticBlockBit(j, i);

                // render changes in static blocks
                if (!shifted && old)
                    ST7789_DrawRectangle(FRAME_X_OFF + j * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + i * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, 0);
                else if (shifted && !old)
                    ST7789_DrawRectangle(FRAME_X_OFF + j * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + i * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, GRAY);
            }
        }

        // clear the top rows
        for (i = ROWS - 1; i <= ROWS - numCleared; i++)
        {
            for (j = 0; j < COLS; j++)
            {
                if (getStaticBlockBit(j, i))
                    ST7789_DrawRectangle(FRAME_X_OFF + j * BLOCK_SIZE + 1,
                    FRAME_Y_OFF + i * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, 0);
            }
        }
    }
}

uint8_t is_dead = 0;

void DrawUI_Thread()
{
    char title[6];
    char numstr[4];

    sprintf(title, "HIGH");
    ST7789_DrawText(&FontStyle_Emulogic, (const char*) &title,
    FRAME_X_OFF - (5 * FONT_WIDTH) - 2 + FONT_WIDTH / 2,
                    FRAME_Y_OFF + 10 * BLOCK_SIZE,
                    ST7789_WHITE,
                    ST7789_BLACK);

    sprintf(title, "SCORE");
    ST7789_DrawText(&FontStyle_Emulogic, (const char*) &title,
    FRAME_X_OFF - (5 * FONT_WIDTH) - 2,
                    FRAME_Y_OFF + 6 * BLOCK_SIZE,
                    ST7789_WHITE,
                    ST7789_BLACK);

    sprintf(title, "LEVEL");
    ST7789_DrawText(&FontStyle_Emulogic, (const char*) &title,
    FRAME_X_OFF - (5 * FONT_WIDTH) - 2,
                    FRAME_Y_OFF + 2 * BLOCK_SIZE,
                    ST7789_WHITE,
                    ST7789_BLACK);

    while (true)
    {
        G8RTOS_WaitSemaphore(&sem_update_ui);

        sprintf(numstr, "%u", highscore);
        ST7789_DrawText(
                &FontStyle_Emulogic,
                (const char*) &numstr,
                FRAME_X_OFF - (5 * FONT_WIDTH) - 2 + FONT_WIDTH * 2
                        - (highscore > 9 ? FONT_WIDTH / 2 : 0)
                        - (highscore > 99 ? FONT_WIDTH / 2 : 0)
                        - (highscore > 999 ? FONT_WIDTH / 2 : 0)
                        - (highscore > 9999 ? FONT_WIDTH / 2 : 0)
                        - (highscore > 99999 ? FONT_WIDTH : 0),
                FRAME_Y_OFF + 9 * BLOCK_SIZE - 2,
                ST7789_WHITE,
                ST7789_BLACK);

        sprintf(numstr, "%u", score);
        ST7789_DrawText(
                &FontStyle_Emulogic,
                (const char*) &numstr,
                FRAME_X_OFF - (5 * FONT_WIDTH) - 2 + FONT_WIDTH * 2
                        - (score > 9 ? FONT_WIDTH / 2 : 0) - (score > 99 ? FONT_WIDTH / 2 : 0)
                        - (score > 999 ? FONT_WIDTH / 2 : 0) - (score > 9999 ? FONT_WIDTH / 2 : 0)
                        - (score > 99999 ? FONT_WIDTH : 0),
                FRAME_Y_OFF + 5 * BLOCK_SIZE - 2,
                ST7789_WHITE,
                ST7789_BLACK);

        sprintf(numstr, "%u", level_num);
        ST7789_DrawText(
                &FontStyle_Emulogic,
                (const char*) &numstr,
                FRAME_X_OFF - (5 * FONT_WIDTH)
                        + (level_num > 9 ? FONT_WIDTH + FONT_WIDTH / 2 : FONT_WIDTH * 2) - 2,
                FRAME_Y_OFF + 1 * BLOCK_SIZE - 2,
                ST7789_WHITE,
                ST7789_BLACK);

        G8RTOS_Yield();
    }
}

uint16_t joy_released = 1;
uint16_t rotate_released = 1;
uint16_t drop_released = 1;
uint16_t hold_released = 1;
uint8_t joy_i = 0;

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
        score += 1;
    }

    uint8_t buttons = MultimodButtons_Get();

    if (buttons & 2)
    {
        if (rotate_released)
        {
            rotate_released = false;
            G8RTOS_WriteFIFO(0, MOVE_ROTATE);
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

            G8RTOS_WriteFIFO(0, MOVE_INSTADROP);
            drop_released = false;
        }
    }
    else
    {
        drop_released = true;
    }

    if (buttons & 8)
    {
        if (hold_released)
        {
            G8RTOS_WriteFIFO(0, MOVE_SWAP);
            G8RTOS_WriteFIFO(0, MOVE_NONE);
            hold_released = false;
        }
    }
    else
    {
        hold_released = true;
    }

    G8RTOS_Yield();

}

void Gravity_P()
{
    if (!resetting)
    {
        G8RTOS_WriteFIFO(0, MOVE_DOWN);
        timer++;
        G8RTOS_Yield();
    }
}

uint8_t staticCheckClear(int8_t row)
{
    if (row >= ROWS)
    {
        abort();
    }
    else if (row < 0)
    {
        return 0;
    }
    for (uint8_t clear_i = 0; clear_i < COLS; clear_i++)
    {
        if (!getStaticBlockBit(clear_i, row))
        {
            return 0;
        }
    }
    return 1;
}

void setStaticBlockBit(int8_t col, int8_t row, int8_t value, uint8_t canLose)
{
    if ((row < 0 || row >= ROWS || col < 0 || col >= COLS) && canLose)
    {
        abort();
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    if (value == 1 && ((static_blocks[byteIndex] >> bitInByte) & 1) && !resetting && canLose)
    {
        abort();
    }

    if (value)
    {
        static_blocks[byteIndex] |= (1 << bitInByte);
    }
    else
    {
        static_blocks[byteIndex] &= ~(1 << bitInByte);
    }
}

uint8_t getStaticBlockBit(int8_t col, int8_t row)
{
    if (row >= ROWS || row < 0 || col < 0 || col >= COLS)
    {
        return 1;
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    return (static_blocks[byteIndex] >> bitInByte) & 1;
}

uint8_t getOldStaticBlockBit(int8_t col, int8_t row)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        return 1;
    }
    int bitIndex = row * COLS + col;
    int byteIndex = bitIndex / BITS_PER_BYTE;
    int bitInByte = bitIndex % BITS_PER_BYTE;

    return (old_static_blocks[byteIndex] >> bitInByte) & 1;
}

void slideStaticBlocks(int8_t row)
{
    if (row < 0 || row >= ROWS)
    {
        abort();
    }

    for (uint8_t r = row; r < ROWS - 1; r++)
    {
        for (uint8_t c = 0; c < COLS; c++)
        {
            setStaticBlockBit(c, r, getStaticBlockBit(c, r + 1), 0);
        }
    }

    for (uint8_t c = 0; c < COLS; c++)
    {
        setStaticBlockBit(c, ROWS - 1, 0, 0);
    }
}

void randomiseGrabBag()
{
    uint8_t ind;

    if (piece_grab_bag[0] == 255)
    {
        for (uint8_t i = NUM_SHAPES; i < NUM_SHAPES * 2; i++)
        {
            piece_grab_bag[i] = 255;
        }
        // initial back gen
        ind = SysTickValueGet() % NUM_SHAPES;
        for (uint8_t i = 0; i < NUM_SHAPES; i++)
        {
            while (piece_grab_bag[NUM_SHAPES + ind] != 255)
            {
                ind = SysTickValueGet() % NUM_SHAPES;
            }

            piece_grab_bag[NUM_SHAPES + ind] = i;
        }
    }

    for (uint8_t i = 0; i < NUM_SHAPES; i++)
    {
        piece_grab_bag[i] = piece_grab_bag[NUM_SHAPES + i];
        piece_grab_bag[NUM_SHAPES + i] = 255;
    }

    ind = SysTickValueGet() % NUM_SHAPES;
    for (uint8_t i = 0; i < NUM_SHAPES; i++)
    {
        while (piece_grab_bag[NUM_SHAPES + ind] != 255)
        {
            ind = SysTickValueGet() % NUM_SHAPES;
        }

        piece_grab_bag[NUM_SHAPES + ind] = i;
    }
}

void renderCrosshatchGrid()
{
    for (uint8_t i = 1; i < COLS; i++)
    {
        ST7789_DrawLine(FRAME_X_OFF + BLOCK_SIZE * i, FRAME_Y_OFF,
        FRAME_X_OFF + BLOCK_SIZE * i,
                        FRAME_Y_OFF + BLOCK_SIZE * ROWS - 1, DARK_GRAY);
        ST7789_DrawLine(FRAME_X_OFF + BLOCK_SIZE * i - 1, FRAME_Y_OFF,
        FRAME_X_OFF + BLOCK_SIZE * i - 1,
                        FRAME_Y_OFF + BLOCK_SIZE * ROWS - 1, DARK_GRAY);
    }

    for (uint8_t i = 1; i < ROWS; i++)
    {
        ST7789_DrawLine(FRAME_X_OFF, FRAME_Y_OFF + BLOCK_SIZE * i,
        FRAME_X_OFF + BLOCK_SIZE * COLS - 1,
                        FRAME_Y_OFF + BLOCK_SIZE * i, DARK_GRAY);

        ST7789_DrawLine(FRAME_X_OFF, FRAME_Y_OFF + BLOCK_SIZE * i - 1,
        FRAME_X_OFF + BLOCK_SIZE * COLS - 1,
                        FRAME_Y_OFF + BLOCK_SIZE * i - 1, DARK_GRAY);
    }
}

/********************************Public Functions***********************************/
