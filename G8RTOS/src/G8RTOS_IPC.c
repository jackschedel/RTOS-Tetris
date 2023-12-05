// G8RTOS_IPC.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for FIFO functions for interprocess communication

#include "../G8RTOS_IPC.h"

/************************************Includes***************************************/

#include "../G8RTOS_Semaphores.h"

#define FIFO_SIZE 16
#define FIFO_COUNT 4

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/

/********************************Private Variables***********************************/

static FIFO_t FIFOs[FIFO_COUNT];

/********************************Public Functions***********************************/

// G8RTOS_InitFIFO
// Initializes FIFO, points head & tail to relevant buffer
// memory addresses. Returns - 1 if FIFO full, 0 if no error
// Param uint32_t "FIFO_index": Index of FIFO block
// Return: int32_t
int32_t G8RTOS_InitFIFO(uint32_t FIFO_index)
{
    if (FIFO_index >= FIFO_COUNT)
        return -1;

    FIFOs[FIFO_index].head = &(FIFOs[FIFO_index].buffer[0]);
    FIFOs[FIFO_index].tail = &(FIFOs[FIFO_index].buffer[0]);
    FIFOs[FIFO_index].lostData = 0;

    G8RTOS_InitSemaphore(&(FIFOs[FIFO_index].mutex), 1);
    G8RTOS_InitSemaphore(&(FIFOs[FIFO_index].currentSize), 0);

    return 0;
}

// G8RTOS_ReadFIFO
// Reads data from head pointer of FIFO.
// Param uint32_t "FIFO_index": Index of FIFO block
// Return: int32_t
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index)
{
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].currentSize));
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].mutex));

    int32_t data = *(FIFOs[FIFO_index].head);

    FIFOs[FIFO_index].head++;

    if (FIFOs[FIFO_index].head >= &(FIFOs[FIFO_index].buffer[FIFO_SIZE]))
        FIFOs[FIFO_index].head = &(FIFOs[FIFO_index].buffer[0]);

    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));

    return data;
}

// G8RTOS_WriteFIFO
// Writes data to tail of buffer.
// 0 if no error, -1 if out of bounds, -2 if full
// Param uint32_t "FIFO_index": Index of FIFO block
// Param uint32_t "data": data to be written
// Return: int32_t
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data)
{
    if (FIFO_index >= FIFO_COUNT)
        return -1;

    if (FIFOs[FIFO_index].currentSize > FIFO_SIZE - 1)
    {
        FIFOs[FIFO_index].lostData++;
        return -2;
    }

    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].mutex));

    *(FIFOs[FIFO_index].tail) = data;
    FIFOs[FIFO_index].tail++;

    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].currentSize));

    if (FIFOs[FIFO_index].tail >= &(FIFOs[FIFO_index].buffer[FIFO_SIZE]))
        FIFOs[FIFO_index].tail = &(FIFOs[FIFO_index].buffer[0]);

    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));

    return 0;
}
