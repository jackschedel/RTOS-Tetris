// multimod_i2c.h
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// i2c header file

#ifndef MULTIMOD_I2C_H_
#define MULTIMOD_I2C_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/i2c.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define I2C_A_BASE          I2C1_BASE
#define I2C_A_GPIO_BASE     GPIO_PORTA_BASE
#define I2C_A_PIN_SCL       GPIO_PIN_6
#define I2C_A_PIN_SDA       GPIO_PIN_7

#define I2C_B_BASE          I2C0_BASE
#define I2C_B_GPIO_BASE     GPIO_PORTB_BASE
#define I2C_B_PIN_SCL       GPIO_PIN_2
#define I2C_B_PIN_SDA       GPIO_PIN_3

#define I2C_C_BASE          I2C0_BASE
#define I2C_C_GPIO_BASE     GPIO_PORTB_BASE
#define I2C_C_PIN_SCL       GPIO_PIN_2
#define I2C_C_PIN_SDA       GPIO_PIN_3

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

void I2C_Init(uint32_t mod);

void I2C_WriteSingle(uint32_t mod, uint8_t addr, uint8_t byte);
void I2C_WriteSingleWaitBetween(uint32_t mod, uint8_t chip_addr,
                                uint8_t write_addr, uint8_t byte);
uint8_t I2C_ReadSingle(uint32_t mod, uint8_t addr);
uint8_t I2C_ReadSingleWaitBetween(uint32_t mod, uint8_t chip_addr,
                                  uint8_t read_addr);

void I2C_WriteMultiple(uint32_t mod, uint8_t addr, uint8_t *data,
                       uint8_t num_bytes);
void I2C_ReadMultiple(uint32_t mod, uint8_t addr, uint8_t *data,
                      uint8_t num_bytes);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_I2C_H_ */

