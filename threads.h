// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_UART;
semaphore_t sem_gameEnd;
semaphore_t sem_clearLine;

/***********************************Semaphores**************************************/

/********************************Thread Functions***********************************/

void Idle_Thread(void);

void FallingBlock_Thread();
void StaticBlocks_Thread();
void DrawUI_Thread();

void FallingBlockGravity_P();
void Get_Joystick_P();

void GPIOE_Handler();

void setStaticBlockBit(int row, int col, int value);
uint8_t getStaticBlockBit(int row, int col);

/********************************Thread Functions***********************************/

#endif /* THREADS_H_ */

