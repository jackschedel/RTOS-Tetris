// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 on sensorpack

#ifndef MULTIMOD_OPT3001_H_
#define MULTIMOD_OPT3001_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

// OPT3001 Defines
#define OPT3001_ADDR                0x47
#define OPT3001_MANUID              0x5449
#define OPT3001_DEVICEID            0x3001

// OPT3001 Register Addresses
#define OPT3001_RESULT_ADDR         0x00
#define OPT3001_CONFIG_ADDR         0x01
#define OPT3001_LOWLIMIT_ADDR       0x02
#define OPT3001_HIGHLIMIT_ADDR      0x03
#define OPT3001_MANUID_ADDR         0x7E
#define OPT3001_DEVICEID_ADDR       0x7F

// OPT3001 Config Register Bitmasks
#define OPT3001_CONFIG_RANGE_BM     0xF000
#define OPT3001_CONFIG_M_BM         0x0600
#define OPT3001_CONFIG_FC_BM        0x0003

// OPT3001 Config Register Shifts
#define OPT3001_CONFIG_RANGE_S      12
#define OPT3001_CONFIG_CT_S         11
#define OPT3001_CONFIG_M_S          9
#define OPT3001_CONFIG_OVF_S        8
#define OPT3001_CONFIG_CRF_S        7
#define OPT3001_CONFIG_FH_S         6
#define OPT3001_CONFIG_FL_S         5
#define OPT3001_CONFIG_L_S          4
#define OPT3001_CONFIG_POL_S        3
#define OPT3001_CONFIG_ME_S         2
#define OPT3001_CONFIG_FC_S         0

// OPT3001 Config Sets
#define OPT3001_CONFIG_CT           0x0800
#define OPT3001_CONFIG_M            0x0600
#define OPT3001_CONFIG_OVF          0x0100
#define OPT3001_CONFIG_CRF          0x0080
#define OPT3001_CONFIG_FH           0x0040
#define OPT3001_CONFIG_FL           0x0020
#define OPT3001_CONFIG_L            0x0010
#define OPT3001_CONFIG_POL          0x0008
#define OPT3001_CONFIG_ME           0x0004

#define OPT3001_FLAG_OVF            0x0100
#define OPT3001_FLAG_CRF            0x0080
#define OPT3001_FLAG_FH             0x0040
#define OPT3001_FLAG_FL             0x0020

// OPT3001 Range Shifts
#define OPT3001_RESULT_E_S          12
#define OPT3001_CONFIG_AUTOFS       0b1100

// OPT3001 Macro
#define LUX(E, R)                   0.01 * (2 << E) * R

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

void OPT3001_Init(void);
void OPT3001_WriteRegister(uint8_t addr, uint16_t data);
uint16_t OPT3001_ReadRegister(uint8_t addr);
uint32_t OPT3001_GetResult(void);
void OPT3001_SetLowLimit(uint16_t exp, uint16_t result);
void OPT3001_SetHighLimit(uint16_t exp, uint16_t result);
uint16_t OPT3001_GetChipID(void);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_OPT3001_H_ */



