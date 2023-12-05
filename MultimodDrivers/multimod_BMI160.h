// multimod_BMI160.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Declarations for BMI160 functions

#ifndef MULTIMOD_BMI160_H_
#define MULTIMOD_BMI160_H_

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

/************************************Includes***************************************/

/*************************************Defines***************************************/

// BMI160 Defines
#define BMI160_ADDR                 0x69
#define BMI160_DEVICEID             0xD1

// BMI160 Addresses
#define BMI160_CHIPID_ADDR          0x00
#define BMI160_ERRREG_ADDR          0x02
#define BMI160_PMUSTATUS_ADDR       0x03
#define BMI160_SENSORTIME_ADDR      0x18
#define BMI160_STATUS_ADDR          0x1B
#define BMI160_FIFODATA_ADDR        0x24
#define BMI160_ACCCONF_ADDR         0x40
#define BMI160_ACCRANGE_ADDR        0x41
#define BMI160_GYRCONFG_ADDR        0x42
#define BMI160_GYRRANGE_ADDR        0x43
#define BMI160_MAGCONF_ADDR         0x44
#define BMI160_FIFODOWNS_ADDR       0x45
#define BMI160_INTOUTCTRL_ADDR      0x53
#define BMI160_INTLATCH_ADDR        0x54
#define BMI160_FOC_CONF_ADDR        0x69
#define BMI160_CONF_ADDR            0x6A
#define BMI160_IFCONF_ADDR          0x6B
#define BMI160_PMUTRIGGER           0x6C
#define BMI160_SELFTEST             0x6D
#define BMI160_NVCONF               0x70
#define BMI160_CMD_ADDR             0x7E
#define BMI160_MAGIC_ADDR           0x7F

// BMI160 Address Offsets
#define BMI160_DATA_O               0x04
#define BMI160_DATA_1               0x05
#define BMI160_INTSTATUS_O          0x1C
#define BMI160_TEMP_O               0x20
#define BMI160_FIFOLENGTH_O         0x22
#define BMI160_FIFOCONFIG_O         0x46
#define BMI160_MAGIF_O              0x4B
#define BMI160_INTEN_O              0x50
#define BMI160_INTMAP_O             0x55
#define BMI160_INTDATA_O            0x58
#define BMI160_INTLOWHIGH_O         0x5A
#define BMI160_INTMOTION_O          0x5F
#define BMI160_INTTAP_O             0x63
#define BMI160_INTORIENT_O          0x65
#define BMI160_INTFLAT_O            0x67
#define BMI160_OFFSET_O             0x71
#define BMI160_STEPCNT_O            0x78
#define BMI160_SETPCONF_O           0x7A

// BMI160 Data Offsets
#define MAGX_O                      0
#define MAGY_O                      2
#define MAGZ_O                      4

#define GYROX_O                     8
#define GYROY_O                     10
#define GYROZ_O                     12

#define ACCELX_O                    14
#define ACCELY_O                    16
#define ACCELZ_O                    18

// BMI160 Status Bits
#define BMI160_STATUS_DRDY_ACC      0x80
#define BMI160_STATUS_DRDY_GYR      0x40
#define BMI160_STATUS_DRDY_MAG      0x20

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

void BMI160_Init(void);
void Mag_Init(void);
void BMI160_WriteRegister(uint8_t addr, uint8_t data);
void Mag_WriteRegister(uint8_t addr, uint8_t data);
uint8_t BMI160_ReadRegister(uint8_t addr);
void BMI160_MultiReadRegister(uint8_t addr, uint8_t *data, uint8_t num_bytes);
int16_t BMI160_TemperatureGetResult();
int16_t BMI160_AccelXGetResult();
int16_t BMI160_AccelYGetResult();
int16_t BMI160_AccelZGetResult();
int16_t BMI160_GyroXGetResult();
int16_t BMI160_GyroYGetResult();
int16_t BMI160_GyroZGetResult();
int16_t BMI160_MagXGetResult();
int16_t BMI160_MagYGetResult();
int16_t BMI160_MagZGetResult();
void BMI160_AccelXYZGetResult(uint16_t *data);
void BMI160_GyroXYZGetResult(uint16_t *data);
void BMI160_MagXYZGetResult(uint16_t *data);
uint8_t BMI160_GetDataStatus();

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_BMI160_H_ */

