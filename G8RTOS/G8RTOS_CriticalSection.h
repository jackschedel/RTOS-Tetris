// G8RTOS_CriticalSection.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Critical section function prototypes. To be defined using assembly.

#ifndef G8RTOS_CRITICALSECTION_H_
#define G8RTOS_CRITICALSECTION_H_

/************************************Includes***************************************/

#include <stdint.h>

#include "G8RTOS_Structures.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/

uint32_t IBit_State;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

extern int32_t StartCriticalSection();
extern void EndCriticalSection(int32_t IBit_State);

/********************************Public Functions***********************************/


#endif /* G8RTOS_CRITICALSECTION_H_ */

