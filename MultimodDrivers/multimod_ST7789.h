// multimod_ST7789.h
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 on sensorpack

#ifndef MULTIMOD_ST7789_H_
#define MULTIMOD_ST7789_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "fontlibrary.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

// ST7789 Defines
#define ST7789_ADDR                 0x47
#define ST7789_MANUID               0x5449
#define ST7789_DEVICEID             0x3001

#define ST7789_PIN_PORT_BASE        GPIO_PORTF_BASE
#define ST7789_CS_PIN               GPIO_PIN_4
#define ST7789_DC_PIN               GPIO_PIN_3

// ST7789 Command Registers
#define ST7789_NOP_ADDR             0x00
#define ST7789_SWRESET_ADDR         0x01
#define ST7789_RDDID_ADDR           0x04
#define ST7789_RDDST_ADDR           0x09
#define ST7789_RDDPM_ADDR           0x0A
#define ST7789_RDDMADCTL_ADDR       0x0B
#define ST7789_RDDCOLMOD_ADDR       0x0C
#define ST7789_RDDIM_ADDR           0x0D
#define ST7789_RDDSM_ADDR           0x0E
#define ST7789_RDDSDR_ADDR          0x0F
#define ST7789_SLPIN_ADDR           0x10
#define ST7789_SLPOUT_ADDR          0x11
#define ST7789_PLTON_ADDR           0x12
#define ST7789_NORON_ADDR           0x13
#define ST7789_INVOFF_ADDR          0x20
#define ST7789_INVON_ADDR           0x21
#define ST7789_GAMSET_ADDR          0x26
#define ST7789_DISPOFF_ADDR         0x28
#define ST7789_DISPON_ADDR          0x29
#define ST7789_CASET_ADDR           0x2A
#define ST7789_RASET_ADDR           0x2B
#define ST7789_RAMWR_ADDR           0x2C
#define ST7789_RAMRD_ADDR           0x2E
#define ST7789_PTLAR_ADDR           0x30
#define ST7789_VSCRDEF_ADDR         0x33
#define ST7789_TEOFF_ADDR           0x34
#define ST7789_TEON_ADDR            0x35
#define ST7789_MADCTL_ADDR          0x36
#define ST7789_VSCRSADD_ADDR        0x37
#define ST7789_IDMOFF_ADDR          0x38
#define ST7789_IDMON_ADDR           0x39
#define ST7789_COLMOD_ADDR          0x3A
#define ST7789_RAMWRC_ADDR          0x3C
#define ST7789_RAMRDC_ADDR          0x3E
#define ST7789_TESCAN_ADDR          0x44
#define ST7789_RDTESCAN_ADDR        0x45
#define ST7789_WRDISBV_ADDR         0x51
#define ST7789_RDDISBV_ADDR         0x52
#define ST7789_WRCTRLD_ADDR         0x53
#define ST7789_RDCTRLD_ADDR         0x54
#define ST7789_WRCACE_ADDR          0x55
#define ST7789_RDCABC_ADDR          0x56
#define ST7789_WRCABCMB_ADDR        0x5E
#define ST7789_RDCABCMB_ADDR        0x5F
#define ST7789_RDABCSDR_ADDR        0x68
#define ST7789_RDID1_ADDR           0xDA
#define ST7789_RDID2_ADDR           0xDB
#define ST7789_RDID3_ADDR           0xDC

// ST7789 Boundaries
#define X_MAX                       240
#define Y_MAX                       320

// COLORS
#define ST7789_BLACK                0x0000
#define ST7789_RED                  0x001F
#define ST7789_BLUE                 0xF800
#define ST7789_GREEN                0x07E0
#define ST7789_YELLOW               0x07FF
#define ST7789_CYAN                 0xFFE0
#define ST7789_WHITE                0xFFFF
#define ST7789_MAGENTA              0xF81F

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

void ST7789_Init();
void ST7789_WriteRegister(uint8_t addr, uint8_t data);
uint8_t ST7789_ReadRegister(uint8_t addr);
void ST7789_Fill(uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ST7789_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawOutline(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawText(const fontStyle_t *font, const char *text, uint16_t x, uint16_t y,
                     uint16_t color, uint16_t bgColor);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* MULTIMOD_ST7789_H_ */

