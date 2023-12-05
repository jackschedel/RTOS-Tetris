// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_buttons.h"

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init()
{

    I2C_Init(I2C_A_BASE);

    uint8_t data[3] = { 0, 0b11110, 0x06 };

    I2C_WriteMultiple(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR, data, 3);

    data[2] = 0x04;
    I2C_WriteMultiple(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR, data, 3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
        ;

    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_4,
    GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t
uint8_t MultimodButtons_Get()
{
    I2C_WriteSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR, 0);
    return ~I2C_ReadSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR) & 0b11110;
}

