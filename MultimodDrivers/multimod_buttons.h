// multimod_buttons.h
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// buttons header file

#ifndef MULTIMOD_BUTTONS_H_
#define MULTIMOD_BUTTONS_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/i2c.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define BUTTONS_PCA9555_GPIO_ADDR   0x23
#define BUTTONS_INT_GPIO_BASE       GPIO_PORTE_BASE
#define BUTTONS_INT_PIN             GPIO_PIN_4

#define SW1                         0x02
#define SW2                         0x04
#define SW3                         0x08
#define SW4                         0x10

#define PCA9555_BUTTONS_ADDR        0x21

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

void MultimodButtons_Init();
uint8_t MultimodButtons_Get();

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_BUTTONS_H_ */


