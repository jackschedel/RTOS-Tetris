// multimod_spi.h
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// spi header file

#ifndef MULTIMOD_SPI_H_
#define MULTIMOD_SPI_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/ssi.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define SPI_A_BASE          SSI0_BASE
#define SPI_A_GPIO_BASE     GPIO_PORTA_BASE
#define SPI_A_PIN_MISO      GPIO_PIN_4
#define SPI_A_PIN_MOSI      GPIO_PIN_5
#define SPI_A_PIN_CLK       GPIO_PIN_2

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

void SPI_Init(uint32_t mod);

void SPI_WriteSingle(uint32_t mod, uint8_t byte);
void SPI_SetMode(uint32_t mod, uint8_t mode);
uint8_t SPI_ReadSingle(uint32_t mod);
void SPI_WriteMultiple(uint32_t mod, uint32_t* data, uint8_t num_bytes);
void SPI_ReadMultiple(uint32_t mod, uint32_t* data, uint8_t num_bytes);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_SPI_H_ */


