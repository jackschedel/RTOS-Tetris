// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"

/************************************Includes***************************************/

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

#include "../G8RTOS_CriticalSection.h"

#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include <string.h>

/************************************Includes***************************************/

#define FAULT_NMI               2           // NMI fault
#define FAULT_HARD              3           // Hard fault
#define FAULT_MPU               4           // MPU fault
#define FAULT_BUS               5           // Bus fault
#define FAULT_USAGE             6           // Usage fault
#define FAULT_SVCALL            11          // SVCall
#define FAULT_DEBUG             12          // Debug monitor
#define FAULT_PENDSV            14          // PendSV
#define FAULT_SYSTICK           15          // System Tick

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
static tcb_t threadControlBlocks[MAX_THREADS];

// Thread Stacks - array of arrays for individual stacks of each thread
static uint32_t threadStacks[MAX_THREADS][STACKSIZE];

// Current Number of Threads currently in the scheduler
static uint32_t NumberOfThreads = 0;

static ptcb_t pthreadControlBlocks[MAX_PTHREADS];
static uint32_t NumberOfPThreads = 0;

/********************************Private Variables**********************************/

/*******************************Private Functions***********************************/

// Occurs every 1 ms.
void InitSysTick(void)
{
    // hint: use SysCtlClockGet() to get the clock speed without having to hardcode it!
    // Set systick period to overflow every 1 ms.
    SysTickPeriodSet(SysCtlClockGet() / 1000);

    // Set systick interrupt handler
    SysTickIntRegister(SysTick_Handler);

    // Set pendsv handler
    IntRegister(FAULT_PENDSV, PendSV_Handler);

    // Enable systick interrupt
    SysTickIntEnable();

    // Enable systick
    SysTickEnable();
}

/*******************************Private Functions***********************************/

/********************************Public Variables***********************************/

tcb_t *CurrentlyRunningThread;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
// Initialize system time to zero
// Set the number of threads to zero
void G8RTOS_Init(void (*idleThread)(void))
{
    IntMasterDisable();

    uint32_t newVTORTable = 0x20000000;
    uint32_t *newTable = (uint32_t*) newVTORTable;
    uint32_t *oldTable = (uint32_t*) 0;

    for (int i = 0; i < 155; i++)
    {
        newTable[i] = oldTable[i];
    }

    HWREG(NVIC_VTABLE) = newVTORTable;

    SystemTime = 0;
    NumberOfThreads = 0;
    NumberOfPThreads = 0;

    // add idle thread with special un-killable id
    G8RTOS_AddThread(idleThread, 255, "idle", 0);
    threadControlBlocks[0].id = 65535;
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch()
{

    // Initialize system tick
    InitSysTick();

    // Set currently running thread to the first control block
    CurrentlyRunningThread = &threadControlBlocks[0];

    // lower value means higher priority
    IntPrioritySet(FAULT_PENDSV, 7);
    IntPrioritySet(FAULT_SYSTICK, 6);

    G8RTOS_Scheduler();

    IntMasterEnable();

    G8RTOS_Start();

    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. Round-robin scheduling.
// Return: void
void G8RTOS_Scheduler()
{
    tcb_t *thread = CurrentlyRunningThread->nextTCB;

    // find the top priority for any valid thread (lowest number)
    uint8_t topPriority = 255;

    for (uint16_t i = 0; i < NumberOfThreads; i++)
    {
        if (isValidThread(thread) && (thread->priority < topPriority))
            topPriority = thread->priority;

        thread = thread->nextTCB;
    }

    // actual scheduling logic now

    thread = CurrentlyRunningThread->nextTCB;

    while (!isValidThread(thread) || thread->priority != topPriority)
    {
        thread = thread->nextTCB;
    }

    CurrentlyRunningThread = thread;
}

// also has side effect of updating asleep bool if past sleep time (for optimization)
bool isValidThread(tcb_t *thread)
{
    if (thread->blocked || !thread->alive)
        return false;

    if (thread->asleep)
    {
        if (thread->sleepCount > SystemTime)
            return false;

        thread->asleep = false;

    }

    return true;
}

sched_ErrCode_t G8RTOS_AddAperiodicEvent(void (*threadToAdd)(void), uint8_t threadPriority,
                                         int32_t IRQn)
{
    int32_t IBit_State = StartCriticalSection();

    if (IRQn < 16 || IRQn > 154)
    {
        EndCriticalSection(IBit_State);
        return IRQn_INVALID;
    }

    if (threadPriority >= 6)
    {
        EndCriticalSection(IBit_State);
        return HWI_PRIORITY_INVALID;
    }

    IntRegister(IRQn, threadToAdd);
    IntPrioritySet(IRQn, (uint32_t) threadPriority);
    IntEnable(IRQn);

    EndCriticalSection(IBit_State);
    return NO_ERROR;
}

sched_ErrCode_t G8RTOS_Add_PeriodicEvent(void (*threadToAdd)(void), uint32_t period,
                                         uint32_t execution, uint16_t id)
{
    int32_t IBit_State = StartCriticalSection();

    if (NumberOfPThreads >= MAX_PTHREADS)
    {
        EndCriticalSection(IBit_State);
        return THREAD_LIMIT_REACHED;
    }

    if (NumberOfPThreads == 0)
    {
        pthreadControlBlocks[NumberOfPThreads].nextPTCB = &pthreadControlBlocks[NumberOfPThreads];
        pthreadControlBlocks[NumberOfPThreads].previousPTCB =
                &pthreadControlBlocks[NumberOfPThreads];
    }
    else
    {
        pthreadControlBlocks[NumberOfPThreads - 1].nextPTCB =
                &pthreadControlBlocks[NumberOfPThreads];
        pthreadControlBlocks[NumberOfPThreads].previousPTCB = &pthreadControlBlocks[NumberOfPThreads
                - 1];

        pthreadControlBlocks[NumberOfPThreads].nextPTCB = &pthreadControlBlocks[0];
        pthreadControlBlocks[0].previousPTCB = &pthreadControlBlocks[NumberOfPThreads];
    }

    pthreadControlBlocks[NumberOfPThreads].functionPointer = threadToAdd;
    pthreadControlBlocks[NumberOfPThreads].period = period;
    pthreadControlBlocks[NumberOfPThreads].execution = SystemTime + execution;
    pthreadControlBlocks[NumberOfPThreads].id = id;

    NumberOfPThreads++;

    EndCriticalSection(IBit_State);
    return NO_ERROR;
}

void G8RTOS_Change_Period(uint16_t id, uint32_t period)
{

    int32_t IBit_State = StartCriticalSection();

    // find thread
    ptcb_t *to_change;
    for (uint8_t i = 0; i < NumberOfPThreads; i++)
    {
        to_change = &pthreadControlBlocks[i];
        if (to_change->id == id)
        {
            to_change->period = period;
            break;
        }

    }

    EndCriticalSection(IBit_State);

}

// G8RTOS_AddThread
// - Adds threads to G8RTOS Scheduler
// - Checks if there are still available threads to insert to scheduler
// - Initializes the thread control block for the provided thread
// - Initializes the stack for the provided thread to hold a "fake context"
// - Sets stack thread control block stack pointer to top of thread stack
// - Sets up the next and previous thread control block pointers in a round robin fashion
// Param void* "threadToAdd": pointer to thread function address
// Param uint8_t threadPriority: priority of the thread - smaller number = higher priority
// Return: scheduler error code
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t threadPriority,
                                 char name[MAX_NAME_LENGTH], uint16_t threadID)
{
    int32_t IBit_State = StartCriticalSection();

    // Check if we can add more threads
    if (NumberOfThreads == MAX_THREADS)
        return THREAD_LIMIT_REACHED;

    uint16_t indexToAdd = NumberOfThreads;

    // idle thread will never die, so no need to check
    for (uint16_t i = 1; i < NumberOfThreads; i++)
    {
        if (!threadControlBlocks[i].alive)
        {
            indexToAdd = i;
            break;
        }
    }

    if (threadID == 65535)
        return INVALID_ID;

    // Initialize the TCB for the new thread
    tcb_t *newThread = &threadControlBlocks[indexToAdd];
    newThread->priority = threadPriority;
    newThread->functionPointer = threadToAdd; // Set function pointer
    newThread->stackPointer = &threadStacks[NumberOfThreads][STACKSIZE - 16]; // Point to the top of the stack
    newThread->alive = true;
    newThread->id = threadID; // currently just doing id = tcb index, it wasnt specified what to set for id.
    strncpy(newThread->name, name, MAX_NAME_LENGTH);
    newThread->name[MAX_NAME_LENGTH] = '\0';
    ((int32_t*) newThread->stackPointer)[14] = (int32_t) threadToAdd; // PC
    ((int32_t*) newThread->stackPointer)[15] = 0x01000000; // xPSR

    // Idle thread base case
    if (indexToAdd == 0)
    {
        newThread->nextTCB = newThread;
        newThread->previousTCB = newThread;
        CurrentlyRunningThread = newThread;
    }
    else
    {
        newThread->nextTCB = CurrentlyRunningThread->nextTCB;
        newThread->previousTCB = CurrentlyRunningThread;
        CurrentlyRunningThread->nextTCB = newThread;

        // note: cool logic verification in camera roll
    }

    NumberOfThreads++;

    EndCriticalSection(IBit_State);

    return NO_ERROR;
}

void G8RTOS_KillThread(uint16_t threadID)
{

    int32_t IBit_State = StartCriticalSection();

    // no killing the idle thread >:(
    if (threadID == 65535)
        return;

    // find thread to kill
    tcb_t *to_kill = CurrentlyRunningThread;
    while (to_kill->id != threadID)
    {
        if (to_kill == CurrentlyRunningThread->previousTCB)
        {
            EndCriticalSection(IBit_State);
            return; // no thread with given ID exists
        }

        to_kill = to_kill->nextTCB;
    }

    to_kill->alive = false;
    to_kill->previousTCB->nextTCB = to_kill->nextTCB;
    to_kill->nextTCB->previousTCB = to_kill->previousTCB;

    EndCriticalSection(IBit_State);

    if (CurrentlyRunningThread->id == threadID)
        G8RTOS_Yield();
}

void G8RTOS_KillSelf()
{
    G8RTOS_KillThread(CurrentlyRunningThread->id);

    G8RTOS_Yield();
}

void G8RTOS_Sleep(uint32_t duration)
{
    CurrentlyRunningThread->asleep = true;

    CurrentlyRunningThread->sleepCount = SystemTime + duration;

    G8RTOS_Yield();
}

void G8RTOS_Yield()
{
// Set PendSV flag
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler()
{

    SystemTime++;

    // should be no need for a critical section

    // todo: find valid starting thread (only necessary once killing periodic threads is a thing)
    ptcb_t *pt = &pthreadControlBlocks[0];

    for (int i = 0; i < NumberOfPThreads; i++)
    {
        if (pt->execution == SystemTime)
        {
            ((void (*)(void)) pt->functionPointer)();
            pt->execution += pt->period;
        }
        pt = pt->nextPTCB;
    }

    G8RTOS_Yield();
}

/********************************Public Functions***********************************/
