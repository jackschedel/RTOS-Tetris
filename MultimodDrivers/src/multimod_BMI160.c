// multimod_BMI160.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for BMI160 functions

/************************************Includes***************************************/

#include "../multimod_BMI160.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// BMI160_Init
// Initializes the BMI160.
// Return: void
void BMI160_Init()
{
    I2C_Init(I2C_A_BASE);

    // Power on accelerometer, gyroscope, and magnetometer
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x11);
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x15);
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x19);

    SysCtlDelay(1000000);

    // configure data rates

    // 100Hz -> 0b1000 -> 0x08
    const uint8_t data_rate = 0x08;

    BMI160_WriteRegister(BMI160_ACCCONF_ADDR, data_rate);
    BMI160_WriteRegister(BMI160_GYRCONFG_ADDR, data_rate);
    //BMI160_WriteRegister(BMI160_MAGCONF_ADDR, data_rate);

    Mag_Init();

    return;
}

// Mag_Init
// Initializes the BMM150 Magnetometer within the BMI160
// Return: void
void Mag_Init()
{
    // Enable Setup mode
    BMI160_WriteRegister(BMI160_MAGIF_O + 1, 0x80);

    // Specify the MAG-sensor register address and data
    Mag_WriteRegister(0x44, 0x06);

    // Specify the time delay
    BMI160_WriteRegister(BMI160_MAGIF_O + 1, 0x00);

    // Specify the data rate
    BMI160_WriteRegister(BMI160_MAGCONF_ADDR, 0x01);

    // Enable Data mode
    BMI160_WriteRegister(BMI160_MAGIF_O + 1, 0x00);

    // Define the lowest address of the register data bytes to read from the MAG-sensor
    BMI160_WriteRegister(BMI160_MAGIF_O + 2, 0x04);

    // Define the register address of the magnetometer to start a measurement in forced mode
    BMI160_WriteRegister(BMI160_MAGIF_O + 3, 0x02);

    // Set MAG mode to "forced"
    BMI160_WriteRegister(0x4C, 0x02);

    // Config MAG read data address to the first data register
    BMI160_WriteRegister(BMI160_MAGIF_O + 2, 0x42);

    // Clear mag_manual_en and set the MAG power to SUSPEND mode
    BMI160_WriteRegister(BMI160_MAGIF_O + 1, 0x03);
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x18);

    return;
}

// BMI160_WriteRegister
// Writes to a register address in the BMI160.
// Param uint8_t "addr": Register address
// Param uint8_t "data": data to write
// Return: void
void BMI160_WriteRegister(uint8_t addr, uint8_t data)
{
    uint8_t writeData[2];

    writeData[0] = addr;
    writeData[1] = data;

    I2C_WriteMultiple(I2C_A_BASE, BMI160_ADDR, writeData, 2);

    return;
}

// Mag_WriteRegister
// Writes to a register address in the magnetometer in the BMI160.
// Param uint8_t "addr": Register address
// Param uint8_t "data": data to write
// Return: void
void Mag_WriteRegister(uint8_t addr, uint8_t data)
{
    // Set the register address for write operation
    BMI160_WriteRegister(BMI160_MAGIF_O + 3, addr);

    // Write the data to the MAG_IF[4]
    BMI160_WriteRegister(BMI160_MAGIF_O + 4, data);

    // Trigger the operation by writing to MAG_IF[2]
    BMI160_WriteRegister(BMI160_MAGIF_O + 2, addr);

    return;
}

// BMI160_ReadRegister
// Reads from a register address in the BMI160.
// Param uint8_t "addr": Register address
// Return: void
uint8_t BMI160_ReadRegister(uint8_t addr)
{

    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);

    return I2C_ReadSingle(I2C_A_BASE, BMI160_ADDR);

}

// BMI160_MultiReadRegister
// Uses the BMI160 auto-increment function to read from multiple registers.
// Param uint8_t "addr": beginning register address
// Param uint8_t* "data": pointer to an array to store data in
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void BMI160_MultiReadRegister(uint8_t addr, uint8_t *data, uint8_t num_bytes)
{

    // Write to the OPT3001 register address
    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);

    // Read from the OPT3001 register
    I2C_ReadMultiple(I2C_A_BASE, BMI160_ADDR, data, num_bytes);

}

// BMI160_TemperatureGetResult
// Gets the 16-bit temperature result.
// Return: int16_t
int16_t BMI160_TemperatureGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
        ;
    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_TEMP_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelXGetResult
// Gets the 16-bit x-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
        ;
    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelYGetResult
// Gets the 16-bit y-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelZGetResult
// Gets the 16-bit z-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroXGetResult
// Gets the 16-bit x-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroYGetResult
// Gets the 16-bit y-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroZGetResult
// Gets the 16-bit z-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagXGetResult
// Gets the 16-bit x-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagYGetResult
// Gets the 16-bit y-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagZGetResult
// Gets the 16-bit z-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
        ;

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelXYZGetResult
// Stores the 16-bit XYZ accelerometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_AccelXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
        ;

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GyroXYZGetResult
// Stores the 16-bit XYZ gyroscope results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_GyroXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
        ;

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_MagXYZGetResult
// Stores the 16-bit XYZ magnetometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_MagXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
        ;

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GetDataStatus
// Gets the status register to determine if data is ready to read.
// Return: uint8_t
uint8_t BMI160_GetDataStatus()
{
    return BMI160_ReadRegister(BMI160_STATUS_ADDR);
}

/********************************Public Functions***********************************/
