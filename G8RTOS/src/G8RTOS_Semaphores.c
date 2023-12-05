// G8RTOS_Semaphores.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for semaphore functions

#include "../G8RTOS_Semaphores.h"

/************************************Includes***************************************/

#include "../G8RTOS_CriticalSection.h"
#include "../G8RTOS_Scheduler.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

/************************************Includes***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

uint32_t IBit_State;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_InitSemaphore
// Initializes semaphore to a value. This is a critical section!
// Param "s": Pointer to semaphore
// Param "value": Value to initialize semaphore to
// Return: void
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
    int32_t IBit_State = StartCriticalSection(); // Start critical section
    *s = value;
    EndCriticalSection(IBit_State); // End critical section
}

// G8RTOS_WaitSemaphore
// Waits on the semaphore to become available, decrements value by 1. This is a
// critical section!
// Currently spin-locks.
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    int32_t IBit_State = StartCriticalSection();
    (*s)--;

    if ((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s;

        EndCriticalSection(IBit_State);

        G8RTOS_Yield();
    }
    else
    {
        EndCriticalSection(IBit_State);
    }
}

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// This is a critical section!
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
    int32_t IBit_State = StartCriticalSection();
    (*s)++;

    if ((*s) < 1)
    {
        tcb_t *thread = CurrentlyRunningThread->nextTCB;

        while (thread->blocked != s)
        {
            thread = thread->nextTCB;
        }

        thread->blocked = 0;
    }

    EndCriticalSection(IBit_State);
}
/********************************Public Functions***********************************/
