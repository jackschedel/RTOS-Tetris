// multimod_ST7789.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for ST7789 TFT display

/************************************Includes***************************************/

#include "../multimod_ST7789.h"

#include "../multimod_spi.h"

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ssi.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/pin_map.h>

/************************************Includes***************************************/

/***********************************Macro Defines***********************************/
#ifndef _swap_int16_t
#define _swap_int16_t(a, b) \
{  \
    int16_t t = a; \
    a = b; \
    b = t; \
}
#endif
/***********************************Macro Defines***********************************/

/********************************Private Functions**********************************/

// ST7789_Select
// Selects the ST7789 for SPI transmission.
// Return: void
void ST7789_Select(void)
{
    GPIOPinWrite(ST7789_PIN_PORT_BASE, ST7789_CS_PIN, 0x00);
}

// ST7789_Deselect
// Deselects the ST7789 for SPI transmission.
// Return: void
void ST7789_Deselect(void)
{
    GPIOPinWrite(ST7789_PIN_PORT_BASE, ST7789_CS_PIN, 0xFF);
}

// ST7789_SetData
// Sets the Data/Command pin to data.
// Return: void
void ST7789_SetData(void)
{
    GPIOPinWrite(ST7789_PIN_PORT_BASE, ST7789_DC_PIN, 0xFF);
}

// ST7789_SetCommand
// Sets the Data/Command pin to command.
// Return: void
void ST7789_SetCommand(void)
{
    GPIOPinWrite(ST7789_PIN_PORT_BASE, ST7789_DC_PIN, 0x00);
}

// ST7789_WriteCommand
// Similar to BMI160, writes to specify which register that data
// will be sent to.
// Param uint8_t "cmd": command register to send data to.
// Return: void
void ST7789_WriteCommand(uint8_t cmd)
{
    ST7789_SetCommand();
    SPI_WriteSingle(SPI_A_BASE, cmd);
    ST7789_SetData();
}

// ST7789_WriteData
// Sends data to register specified by CMD.
// Param uint8_t "data": data to be sent.
// Return: void
void ST7789_WriteData(uint8_t data)
{
    SPI_WriteSingle(SPI_A_BASE, data);
}

// ST7789_ReadRegister
// Reads from SPI bus.
// Return: uint8_t
uint8_t ST7789_ReadRegister(uint8_t data)
{
    return SPI_ReadSingle(SPI_A_BASE);
}

// ST7789_SetWindow
// Sets windows subsequent pixels will be generated at.
// Param int16_t x: x-coord of first corner.
// Param int16_t y: y-coord of first corner.
// Param int16_t w: width of window.
// Param int16_t h: height of window.
// Return: void
void ST7789_SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (x < 0)
    {
        x = 0;
    }

    if (y < 0)
    {
        y = 0;
    }

    // offset y by 20
    y += 20;

    ST7789_WriteCommand(ST7789_CASET_ADDR);
    ST7789_WriteData((x >> 8) & 0xFF);
    ST7789_WriteData((x >> 0) & 0xFF);
    ST7789_WriteData(((x + w - 1) >> 8) & 0xFF);
    ST7789_WriteData(((x + w - 1) >> 0) & 0xFF);

    ST7789_WriteCommand(ST7789_RASET_ADDR);
    ST7789_WriteData((y >> 8) & 0xFF);
    ST7789_WriteData((y >> 0) & 0xFF);
    ST7789_WriteData(((y + h - 1) >> 8) & 0xFF);
    ST7789_WriteData(((y + h - 1) >> 0) & 0xFF);

    ST7789_WriteCommand(ST7789_RAMWR_ADDR);
}

// ST7789_DrawVLine
// Draws a vertical line.
// Param int16_t x: x-coord of first pixel.
// Param int16_t y: y-coord of first pixel.
// Param uint16_t h: height of line.
// Param uint16_t color: color of line.
// Return: void
void ST7789_DrawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    if ((x < X_MAX) && (y < Y_MAX) && h)
    {
        uint8_t color_hi = color >> 8, color_lo = color;
        if ((y + h - 1) >= Y_MAX)
            h = Y_MAX - y;
        ST7789_Select();
        ST7789_SetWindow(x, y, 1, h);
        while (h--)
        {
            ST7789_WriteData(color_hi);
            ST7789_WriteData(color_lo);
        }

        ST7789_Deselect();
    }
}

// ST7789_DrawHLine
// Draws a horizontal line.
// Param int16_t x: x-coord of first pixel.
// Param int16_t y: y-coord of first pixel.
// Param uint16_t w: width of line.
// Param uint16_t color: color of line.
// Return: void
void ST7789_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    if ((x < X_MAX) && (y < Y_MAX) && w)
    {
        uint8_t color_hi = color >> 8, color_lo = color;

        if ((x >= X_MAX) || (y >= Y_MAX))
            return;
        if ((x + w - 1) >= X_MAX)
            w = X_MAX - x;
        ST7789_Select();
        ST7789_SetWindow(x, y, w, 1);
        while (w--)
        {
            ST7789_WriteData(color_hi);
            ST7789_WriteData(color_lo);
        }

        ST7789_Deselect();
    }
}

// ST7789_Line
// Draws a line from point 1 to point 2.
// Param uint16_t x0: x-coord of first point.
// Param uint16_t y0: y-coord of first point.
// Param uint16_t x1: x-coord of second point.
// Param uint16_t y1: y-coord of second point.
// Param uint16_t color: color of line.
// Return: void
void ST7789_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    bool steep = abs((int16_t) (y1 - y0)) > abs((int16_t) (x1 - x0));
    if (steep)
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs((int16_t) (y1 - y0));

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            ST7789_DrawPixel(y0, x0, color);
        }
        else
        {
            ST7789_DrawPixel(x0, y0, color);
        }

        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

// delay_ms
// Software delay.
// Param: uint32_t "ms": number of milliseconds to delay.
// Return: void
void delay_ms(uint32_t ms)
{
    SysCtlDelay((SysCtlClockGet() / (3 * 1000)) * ms + 1);
}

/********************************Private Functions**********************************/

/********************************Public Functions***********************************/

// ST7789_Init
// Initializes the TFT display to begin being able to be drawn to.
// Return: void
void ST7789_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(ST7789_PIN_PORT_BASE, ST7789_CS_PIN);
    GPIOPinTypeGPIOOutput(ST7789_PIN_PORT_BASE, ST7789_DC_PIN);
    SPI_Init(SPI_A_BASE);

    ST7789_Deselect();

    // Do software reset first
    ST7789_Select();

    ST7789_WriteCommand(ST7789_SWRESET_ADDR);
    delay_ms(120);
    ST7789_WriteCommand(ST7789_SLPOUT_ADDR);
    delay_ms(500);
    ST7789_WriteCommand(ST7789_COLMOD_ADDR);
    ST7789_WriteData(0x55);

    ST7789_WriteCommand(ST7789_CASET_ADDR);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x00);
    ST7789_WriteData((X_MAX >> 8 & 0xFF));
    ST7789_WriteData((X_MAX >> 0 & 0xFF));

    ST7789_WriteCommand(ST7789_RASET_ADDR);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x00);
    ST7789_WriteData((Y_MAX >> 8 & 0xFF));
    ST7789_WriteData((Y_MAX >> 0 & 0xFF));

    ST7789_WriteCommand(ST7789_RASET_ADDR);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x00);
    ST7789_WriteData((Y_MAX >> 8 & 0xFF));
    ST7789_WriteData((Y_MAX >> 0 & 0xFF));

    // set column address order right to left, line address order bottom to top
    ST7789_WriteCommand(ST7789_MADCTL_ADDR);
    ST7789_WriteData(0b01001000);

    ST7789_WriteCommand(ST7789_NORON_ADDR);
    delay_ms(120);

    ST7789_WriteCommand(ST7789_INVON_ADDR);
    delay_ms(120);

    ST7789_WriteCommand(ST7789_DISPON_ADDR);
    delay_ms(120);
    ST7789_Fill(0x0000);
    ST7789_Deselect();
}

// ST7789_DrawPixel
// Draws a pixel of color at x, y.
// Param uint16_t "x": x-axis of pixel.
// Param uint16_t "y": y-axis of pixel.
// Param uint16_t "color": color of pixel.
// Return: void
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x < X_MAX && y < Y_MAX)
    {
        ST7789_Select();
        ST7789_SetWindow(x, y, 1, 1);
        ST7789_WriteData(color >> 8);
        ST7789_WriteData(color & 0xFF);
        ST7789_Deselect();
    }
}

// ST7789_DrawPixel
// Fills screen with color.
// Param uint16_t "color": color of fill.
// Return: void
void ST7789_Fill(uint16_t color)
{
    ST7789_DrawRectangle(0, 0, X_MAX, Y_MAX, color);
}

// ST7789_DrawLine
// Draws a line from point 1 to point 2.
// Param uint16_t x0: x-coord of first point.
// Param uint16_t y0: y-coord of first point.
// Param uint16_t x1: x-coord of second point.
// Param uint16_t y1: y-coord of second point.
// Param uint16_t color: color of line.
// Return: void
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    // Update in subclasses if desired!
    if (x0 == x1)
    {
        if (y0 > y1)
            _swap_int16_t(y0, y1);
        ST7789_DrawVLine(x0, y0, y1 - y0 + 1, color);
    }
    else if (y0 == y1)
    {
        if (x0 > x1)
            _swap_int16_t(x0, x1);
        ST7789_DrawHLine(x0, y0, x1 - x0 + 1, color);
    }
    else
    {
        ST7789_Line(x0, y0, x1, y1, color);
    }
}

// ST7789_DrawRectangle
// Draws a rectangle.
// Param uint16_t x: x-coord of first point.
// Param uint16_t y: y-coord of first point.
// Param uint16_t w: width of rectangle
// Param uint16_t h: height of retangle.
// Param uint16_t color: color of line.
// Return: void
void ST7789_DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    uint8_t color_hi, color_lo;
    ST7789_Select();

    if (x < 0)
    {
        w += x;
    }

    if (x + w > X_MAX)
    {
        w -= x + w - X_MAX;
    }

    if (y < 0)
    {
        h += y;
    }

    if (y + h > Y_MAX)
    {
        h -= y + h - Y_MAX;
    }

    ST7789_SetWindow(x, y, w, h);

    color_hi = color >> 8;
    color_lo = color & 0xFF;

    uint32_t num_p = (uint32_t) w * (uint32_t) h;
    while (num_p--)
    {
        ST7789_WriteData(color_hi);
        ST7789_WriteData(color_lo);
    }
    ST7789_Deselect();
}

/********************************Public Functions***********************************/

