/*!
 * @file FT6206.c
 */

#include "main.h"
#include "i2c.h"
#include "FT6206.h"

#include <stdio.h>
#include <string.h>

#define FT6206_DEBUG
//#define I2C_DEBUG

/*
 *  Global variables
 */
uint8_t  TS_rotation;    // Display rotation (0 thru 3)
uint16_t TS_width;       // Display width as modified by current rotation
uint16_t TS_height;      // Display height as modified by current rotation

uint8_t FT6206_touched;
uint16_t FT6206_touchX;
uint16_t FT6206_touchY;


/*
 * Private Variables
 */
static uint8_t i2cdata[16];

/*
 * Private Function Prototypes
 */
static uint8_t FT6206_readRegister8( uint8_t reg );
static void FT6206_writeRegister8( uint8_t reg, uint8_t val );

/*
 *  -------------------
 *  Public Functions
 * -------------------
 */

/**
 *  Setups the I2C interface and hardware, identifies if chip is found.
 *
 *  @param  thresh Optional threshhold-for-touch value, default is
 *  FT6206_DEFAULT_THRESSHOLD but you can try changing it if your screen is
 *  too/not sensitive.
 *
 *  @returns 0 if an FT6206 is found, non=zero on any failure
 */
int8_t FT6206_init( uint8_t thresh )
{
#ifdef FT6206_DEBUG

    printf("FT6206_DEBUG\r\n");
    printf("  Vend ID:       0x%02X\r\n", FT6206_readRegister8(FT6206_REG_VENDID));
    printf("  Chip ID:       0x%02X\r\n", FT6206_readRegister8(FT6206_REG_CHIPID));
    printf("  Firm V:        0x%02X\r\n", FT6206_readRegister8(FT6206_REG_FIRMVERS));
    printf("  Point Rate Hz: 0x%02X\r\n", FT6206_readRegister8(FT6206_REG_POINTRATE));
    printf("  THRESH:        0x%02X\r\n", FT6206_readRegister8(FT6206_REG_THRESHHOLD));

    // dump all registers
  for (int16_t i = 0; i < 0x10; i++)
  {
      printf("  REG $%X = 0x%02X\r\n", i, FT6206_readRegister8(i) );
  }
#endif

    // change threshold to be higher/lower
    FT6206_writeRegister8( FT6206_REG_THRESHHOLD, thresh );

    if ( FT6206_readRegister8( FT6206_REG_VENDID ) != FT6206_VAL_VENDID ||
         FT6206_readRegister8( FT6206_REG_CHIPID ) != FT6206_VAL_CHIPID )
    {
        return 1; // failure
    }

    FT6206_touched = 0;
    FT6206_touchX = 0;
    FT6206_touchY = 0;
    FT6206_setRotation(0);

    return 0; // OK
}

/**
 * Reads the bulk of data from cap touch chip. Fills in FT6206_touched,
 * FT6206_touchX, and FT6206_touchY with results. The X and Y coordinates are adjusted
 * per the current setting of TS_rotation.
 */
void FT6206_readData( void )
{
    // Read: 1 address byte, 7 data bytes, 1000mSec timeout
    HAL_I2C_Mem_Read( &FT6206_I2C_HANDLE, FT6206_ADDR, 0x00, 1, i2cdata, 7, 1000 );

    FT6206_touched = (i2cdata[0x02] & 0x0F) ? 1 : 0;
    uint16_t x = (( i2cdata[0x03] & 0x0F ) << 8) + ( i2cdata[0x04] );
    uint16_t y = (( i2cdata[0x05] & 0x0F ) << 8) + ( i2cdata[0x06] );

    switch(TS_rotation)
    {
        case 0:
            // Noon
            FT6206_touchX = FT6206_WIDTH - x;
            FT6206_touchY = FT6206_HEIGHT - y;
            break;
        case 1:
            // 3 o'clock
            FT6206_touchX = FT6206_HEIGHT - y;
            FT6206_touchY = x;
            break;
        case 2:
            // 6 o'clock
            FT6206_touchX = x;
            FT6206_touchY = y;
            break;
        case 3:
            FT6206_touchX = y;
            FT6206_touchY = FT6206_WIDTH - x;
            break;
    }
}

/**
 * Set origin of orientation of Touch Screen.
 *
 * @param   m  The index for rotation, from 0-3 inclusive
 */
void FT6206_setRotation(uint8_t m)
{
    TS_rotation = m & 0x03;     // force to 0 to 3
    switch (TS_rotation)
    {
    case 0:
    case 2:
        TS_width = FT6206_WIDTH;
        TS_height = FT6206_HEIGHT;
        break;
    case 1:
    case 3:
        TS_width = FT6206_HEIGHT;
        TS_height = FT6206_WIDTH;
        break;
    }
}

/**
 * -------------------
 *  Private Functions
 * -------------------
 */

static uint8_t FT6206_readRegister8( uint8_t reg )
{
    //HAL_StatusTypeDef ret;

    uint8_t val;

//  // use i2c
//  Wire.beginTransmission(FT6206_ADDR);
//  Wire.write((byte)reg);
//  Wire.endTransmission();
//
//  Wire.requestFrom((byte)FT6206_ADDR, (byte)1);
//  val = Wire.read();

//    HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
//                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
    HAL_I2C_Mem_Read( &FT6206_I2C_HANDLE, FT6206_ADDR, reg, 1, &val, 1, 1000 );

#ifdef I2C_DEBUG
  Serial.print("$");
  Serial.print(reg, HEX);
  Serial.print(": 0x");
  Serial.println(val, HEX);
#endif

    return val;
}

static void FT6206_writeRegister8( uint8_t reg, uint8_t val )
{
//  // use i2c
//  Wire.beginTransmission(FT6206_ADDR);
//  Wire.write((byte)reg);
//  Wire.write((byte)val);
//  Wire.endTransmission();

    HAL_I2C_Mem_Write( &FT6206_I2C_HANDLE, FT6206_ADDR, reg, 1, &val, 1, 1000 );
}
