/**
 * @file    ts.c
 * @brief   Driver for the FT6206 Touch Screen on the 2.8" TFT LCD Display (Adafruit # 2090)
 */

/**
 ******************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 John Vedder
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************
 * Portions adapted from Adafruit Arduino Libraries. See LICENSE file.
 ******************************************************************************
 */

#include "main.h"
#include "i2c.h"
#include "ts.h"
#include <stdio.h>
#include <string.h>

#define TS_DEBUG

/*
 *  Global variables
 */
uint8_t TS_rotation;    // Display orientation rotation (0 thru 3)
uint16_t TS_width;       // Display width as modified by current rotation
uint16_t TS_height;      // Display height as modified by current rotation

uint8_t TS_isTouched;    // non-zero if touch screen is being touched
uint16_t TS_touchX;  // x coordinate of current touch after accounting for current rotation
uint16_t TS_touchY;  // y coordinate of current touch after accounting for current rotation

/*
 * Private Variables
 */
static uint8_t i2cdata[16];

/*
 * Private Function Prototypes
 */
static uint8_t TS_ReadRegister8( uint8_t reg );
static void TS_WriteRegister8( uint8_t reg, uint8_t val );

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
int8_t TS_Init( uint8_t thresh )
{

#ifdef TS_DEBUG
    printf( "TS_DEBUG\r\n" );
    printf( "  Vend ID:       0x%02X\r\n",
            TS_ReadRegister8( FT6206_REG_VENDID ) );
    printf( "  Chip ID:       0x%02X\r\n",
            TS_ReadRegister8( FT6206_REG_CHIPID ) );
    printf( "  Firm V:        0x%02X\r\n",
            TS_ReadRegister8( FT6206_REG_FIRMVERS ) );
    printf( "  Point Rate Hz: 0x%02X\r\n",
            TS_ReadRegister8( FT6206_REG_POINTRATE ) );
    printf( "  THRESH:        0x%02X\r\n",
            TS_ReadRegister8( FT6206_REG_THRESHHOLD ) );
#endif

    // change threshold to be higher/lower
    TS_WriteRegister8( FT6206_REG_THRESHHOLD, thresh );

    if ( TS_ReadRegister8( FT6206_REG_VENDID ) != FT6206_VAL_VENDID
            || TS_ReadRegister8( FT6206_REG_CHIPID ) != FT6206_VAL_CHIPID )
    {
        return 1;  // failure
    }

    TS_isTouched = 0;
    TS_touchX = 0;
    TS_touchY = 0;
    TS_SetRotation( 0 );

    return 0;  // OK
}

/**
 * Reads the bulk of data from cap touch chip. Fills in FT6206_touched,
 * FT6206_touchX, and FT6206_touchY with results. The X and Y coordinates are adjusted
 * per the current setting of TS_rotation.
 */
void TS_ReadData( void )
{
    // Read: 1 address byte, 7 data bytes, 1000mSec timeout
    HAL_I2C_Mem_Read( &FT6206_I2C_HANDLE, FT6206_ADDR, 0x00, 1, i2cdata, 7,
            1000 );

    TS_isTouched = ( i2cdata[0x02] & 0x0F ) ? 1 : 0;
    uint16_t x = ( ( i2cdata[0x03] & 0x0F ) << 8 ) + ( i2cdata[0x04] );
    uint16_t y = ( ( i2cdata[0x05] & 0x0F ) << 8 ) + ( i2cdata[0x06] );

    switch ( TS_rotation )
    {
        case 0:
            // Noon
            TS_touchX = TS_WIDTH - x;
            TS_touchY = TS_HEIGHT - y;
            break;
        case 1:
            // 3 o'clock
            TS_touchX = TS_HEIGHT - y;
            TS_touchY = x;
            break;
        case 2:
            // 6 o'clock
            TS_touchX = x;
            TS_touchY = y;
            break;
        case 3:
            TS_touchX = y;
            TS_touchY = TS_WIDTH - x;
            break;
    }
}

/**
 * Set origin of orientation of Touch Screen.
 *
 * @param   m  The index for rotation, from 0-3 inclusive
 */
void TS_SetRotation( uint8_t m )
{
    TS_rotation = m & 0x03;     // force to 0 to 3
    switch ( TS_rotation )
    {
        case 0:
        case 2:
            TS_width = TS_WIDTH;
            TS_height = TS_HEIGHT;
            break;
        case 1:
        case 3:
            TS_width = TS_HEIGHT;
            TS_height = TS_WIDTH;
            break;
    }
}

/**
 * -------------------
 *  Private Functions
 * -------------------
 */

static uint8_t TS_ReadRegister8( uint8_t reg )
{
    uint8_t val;
    HAL_I2C_Mem_Read( &FT6206_I2C_HANDLE, FT6206_ADDR, reg, 1, &val, 1, 1000 );
    return val;
}

static void TS_WriteRegister8( uint8_t reg, uint8_t val )
{
    HAL_I2C_Mem_Write( &FT6206_I2C_HANDLE, FT6206_ADDR, reg, 1, &val, 1, 1000 );
}
