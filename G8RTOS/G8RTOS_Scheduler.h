// G8RTOS_Scheduler.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Scheduler / initialization code for G8RTOS

#ifndef G8RTOS_SCHEDULER_H_
#define G8RTOS_SCHEDULER_H_

/************************************Includes***************************************/

#include <stdint.h>

#include "G8RTOS_Structures.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

#define MAX_THREADS 4
#define MAX_PTHREADS 2
#define STACKSIZE 1024
#define OSINT_PRIORITY 7

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/

// Scheduler error typedef
typedef enum
{
    NO_ERROR = 0,
    THREAD_LIMIT_REACHED = -1,
    NO_THREADS_SCHEDULED = -2,
    THREADS_INCORRECTLY_ALIVE = -3,
    THREAD_DOES_NOT_EXIST = -4,
    CANNOT_KILL_LAST_THREAD = -5,
    IRQn_INVALID = -6,
    HWI_PRIORITY_INVALID = -7,
    INVALID_ID = -8
} sched_ErrCode_t;

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/

extern tcb_t *CurrentlyRunningThread;

uint32_t SystemTime;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

extern void G8RTOS_Start();
extern void PendSV_Handler();

void G8RTOS_Init(void (*idleThread)(void));
void EmptyThreadWait(void);
int32_t G8RTOS_Launch(void);
void G8RTOS_Scheduler();
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t threadPriority, char name[32],
                                 uint16_t threadID);
void SysTick_Handler();
void G8RTOS_Sleep(uint32_t duration);
void G8RTOS_Yield();

bool isValidThread(tcb_t *thread);
void G8RTOS_KillThread(uint16_t threadID);
void G8RTOS_KillSelf();

sched_ErrCode_t G8RTOS_AddAperiodicEvent(void (*threadToAdd)(void), uint8_t threadPriority,
                                         int32_t IRQn);

sched_ErrCode_t G8RTOS_Add_PeriodicEvent(void (*threadToAdd)(void), uint32_t period,
                                         uint32_t execution);

/********************************Public Functions***********************************/

#endif /* G8RTOS_SCHEDULER_H_ */
