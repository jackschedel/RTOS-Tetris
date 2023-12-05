// multimod_joystic.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Declarations for multimod joystick functions

#ifndef MULTIMOD_JOYSTICK_H_
#define MULTIMOD_JOYSTICK_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define JOYSTICK_INT_GPIO_BASE       GPIO_PORTD_BASE
#define JOYSTICK_INT_PIN             GPIO_PIN_2

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void JOYSTICK_Init(void);
void JOYSTICK_IntEnable(void);
uint8_t JOYSTICK_GetPress(void);
uint16_t JOYSTICK_GetX(void);
uint16_t JOYSTICK_GetY(void);
uint32_t JOYSTICK_GetXY(void);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_JOYSTICK_H_ */



