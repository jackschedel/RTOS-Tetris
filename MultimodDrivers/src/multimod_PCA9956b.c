// multimod_PCA9956b.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for PCA9956b functions

/************************************Includes***************************************/

#include "../multimod_PCA9956b.h"

#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// PCA9956b_Init
// Initializes the PCA9956b, initializes the relevant output enable pins
// Return: void
void PCA9956b_Init()
{
    // Initialize I2C module
    I2C_Init(I2C_A_BASE);

    // not working regardless of whether driven high or low
    // Configure relevant Output Enable pin PE4
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);

    PCA9956b_SetAllMax();

    return;
}

// PCA9956b_SetAllMax
// Writes to the IREFALL, PWMALL registers, sets LEDs to maximum.
// Return: void
void PCA9956b_SetAllMax()
{
    // Set IREF to max
    PCA9956b_WriteRegister(0x40, 0xff);

    // Set PWM to max
    PCA9956b_WriteRegister(0x3f, 0xff);

}

uint8_t PCA9956b_GetResult()
{
    return PCA9956b_ReadRegister(0x02);
}

// PCA9956b_SetAllOff
// Writes to the IREFALL, PWMALL registers, sets LEDs to off.
// Return: void
void PCA9956b_SetAllOff()
{
    // Set IREF to 0
    // Set PWM to 0
}

uint8_t PCA9956b_GetChipID()
{
    // idk if this register is right
    return PCA9956b_ReadRegister(0x3b);
}

void PCA9956b_WriteRegister(uint8_t addr, uint8_t data)
{

    uint8_t writeData[2];

    writeData[0] = addr;
    writeData[1] = data;

    I2C_WriteMultiple(I2C_A_BASE, PCA9956B_ADDR, writeData, 2);

    return;
}

uint8_t PCA9956b_ReadRegister(uint8_t addr)
{

    I2C_WriteSingle(I2C_A_BASE, PCA9956B_ADDR, addr);

    return I2C_ReadSingle(I2C_A_BASE, PCA9956B_ADDR);

}

/********************************Public Functions***********************************/
