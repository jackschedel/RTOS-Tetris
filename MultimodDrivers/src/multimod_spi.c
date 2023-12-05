// multimod_spi.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for SPI functions

/************************************Includes***************************************/

#include "../multimod_spi.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_ssi.h>

/********************************Public Functions***********************************/

// SPI_Init
// Initializes specified SPI module. By default the mode
// is set to communicate with the TFT display.
// Param uint32_t "mod": base address of module
// Return: void
void SPI_Init(uint32_t mod)
{

    if (mod == SPI_A_BASE)
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

        GPIOPinConfigure(GPIO_PA2_SSI0CLK);
        // GPIOPinConfigure(GPIO_PA3_SSI0FSS);
        GPIOPinConfigure(GPIO_PA4_SSI0RX);
        GPIOPinConfigure(GPIO_PA5_SSI0TX);

        GPIOPinTypeSSI(SPI_A_GPIO_BASE, SPI_A_PIN_MISO | SPI_A_PIN_MOSI | SPI_A_PIN_CLK);

        SSIDisable(mod);
        SSIClockSourceSet(mod, SSI_CLOCK_SYSTEM);
        SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER,
                           15000000, 8);
        SSIEnable(mod);
    }
}

// SPI_WriteSingle
// Writes a single byte over the SPI line.
// No need to declare an address as in I2C as
// the code is expected to select the chip prior
// to calling this function.
// Param uint32_t "mod": base address of module
// Param uint8_t "byte": byte to send
// Return: void
void SPI_WriteSingle(uint32_t mod, uint8_t byte)
{
    SSIDataPut(mod, byte);
    while (SSIBusy(mod))
        ;
    return;
}

// SPI_ReadSingle
// Reads a single byte from SSI module.
// Param uint32_t "mod": base address of module
// Return: uint8_t
uint8_t SPI_ReadSingle(uint32_t mod)
{
    uint32_t result = 0;

    SSIDataPut(mod, 0x00);
    while (SSIBusy(mod))
        ;

    SSIDataGet(mod, &result);

    return result;
}

// SPI_WriteMultiple
// Write multiple bytes to a device.
// Param uint32_t "mod": base address of module
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to transmit
// Return: void
void SPI_WriteMultiple(uint32_t mod, uint32_t *data, uint8_t num_bytes)
{
    SSIDataPut(mod, *data++);
    num_bytes--;
    while (SSIBusy(mod))
        ;

    while (num_bytes > 0)
    {
        SSIDataPut(mod, *data++);
        num_bytes--;
        while (SSIBusy(mod))
            ;
    }
}

// SPI_ReadMultiple
// Read multiple bytes from a device.
// Param uint32_t "mod": base address of module
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void SPI_ReadMultiple(uint32_t mod, uint32_t *data, uint8_t num_bytes)
{
    SSIDataPut(mod, 0x00);
    num_bytes--;
    while (SSIBusy(mod))
        ;
    SSIDataGet(mod, data++);

    while (num_bytes > 0)
    {
        SSIDataPut(mod, 0x00);
        num_bytes--;
        while (SSIBusy(mod))
            ;
        SSIDataGet(mod, data++);
    }

    return;
}
