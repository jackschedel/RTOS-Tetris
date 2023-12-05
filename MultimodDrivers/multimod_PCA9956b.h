// multimod_PCA9956b.h
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// LED driver header file

#ifndef MULTIMOD_PCA9956B_H_
#define MULTIMOD_PCA9956B_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/i2c.h>
#include <driverlib/gpio.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

// PCA9956B chip addr
#define PCA9956B_ADDR               0x55

// Auto-increment flag, OR with register address
#define PCA9956B_AIF_F              0x80

#define PCA9956B_OE_PIN             GPIO_PIN_2

// PCA9956B Register Addresses
#define PCA9956B_MODE1_ADDR         0x00
#define PCA9956B_MODE2_ADDR         0x01
#define PCA9956B_LEDOUT0_ADDR       0x02
#define PCA9956B_GRPPWM_ADDR        0x08
#define PCA9956B_GRPFREQ_ADDR       0x09
#define PCA9956B_PWM0_ADDR          0x0A
#define PCA9956B_IREF0_ADDR         0x22
#define PCA9956B_OFFSET_ADDR        0x3A
#define PCA9956B_SUBADR1_ADDR       0x3B
#define PCA9956B_ALLCALLADR_ADDR    0x3E
#define PCA9956B_PWMALL_ADDR        0x3F
#define PCA9956B_IREFALL_ADDR       0x40
#define PCA9956B_ELFLAG0_ADDR       0x41

// MODE1 CONFIG BITS
#define PCA9956_MODE1_AI_ALL        (0x00 << 5)
#define PCA9956_MODE1_AI_INDVBRIGHT (0x01 << 5)
#define PCA9956_MODE1_AI_MODETOIREF (0x02 << 5)
#define PCA9956_MODE1_AI_GLOBBRIGHT (0x03 << 5)

#define PCA9956_MODE1_SLEEP         0x10

#define PCA9956_MODE1_SUB1          0x08
#define PCA9956_MODE1_SUB2          0x04
#define PCA9956_MODE1_SUB3          0x02

#define PCA9956_MODE1_ALLCALL       0x01

// add your own defines if needed

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

void PCA9956b_Init(void);
void PCA9956b_SetAllMax(void);
void PCA9956b_SetAllOff(void);
void PCA9956b_EnableOutput(void);
void PCA9956b_DisableOutput(void);

uint8_t PCA9956b_GetChipID(void);
uint8_t PCA9956b_GetResult(void);

void PCA9956b_WriteRegister(uint8_t addr, uint8_t data);
uint8_t PCA9956b_ReadRegister(uint8_t addr);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_PCA9956B_H_ */

