// G8RTOS_IPC.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Interprocess communication code for G8RTOS

#ifndef G8RTOS_IPC_H_
#define G8RTOS_IPC_H_

/************************************Includes***************************************/

#include <stdint.h>

#include "./G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define FIFO_SIZE 16
#define MAX_NUMBER_OF_FIFOS 2

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
typedef struct FIFO_t
{
    int32_t buffer[FIFO_SIZE];
    int32_t *head;
    int32_t *tail;
    uint32_t lostData;
    semaphore_t currentSize;
    semaphore_t mutex;
} FIFO_t;
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

int32_t G8RTOS_InitFIFO(uint32_t FIFO_index);
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index);
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data);
uint8_t G8RTOS_FIFO_Empty(uint32_t FIFO_index);

/********************************Public Functions***********************************/

#endif /* G8RTOS_IPC_H_ */

