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
semaphore_t sem_buttons;
semaphore_t sem_jumping;
semaphore_t sem_died;
semaphore_t sem_reset;
semaphore_t sem_4;

/***********************************Semaphores**************************************/

/********************************Thread Functions***********************************/

void EmptyThreadWait();

void Idle_Thread(void);

void Obstacle_Thread();
void CamMove_Thread();
void Cube_Thread();
void Read_Buttons_Thread();
void Read_JoystickPress_Thread();
void Dino_Thread();

void Print_WorldCoords_P();
void Get_Joystick_P();
void Reset_Thread();

void GPIOE_Handler();
void GPIOD_Handler();

/********************************Thread Functions***********************************/

#endif /* THREADS_H_ */

