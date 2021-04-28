/**
 * @file    ts.c
 * @brief   Header file for driver for the FT6206 Touch Screen on the 2.8" TFT LCD Display (Adafruit # 2090)
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

#ifndef _TS_H
#define _TS_H

#define TS_WIDTH                (240)   // Touch Screen Width
#define TS_HEIGHT               (320)   // Touch Screen Height

#define FT6206_ADDR             (0x70)  // I2C address

#define FT6206_NUM_X            (0x33)  // Touch X position
#define FT6206_NUM_Y            (0x34)  // Touch Y position

#define FT6206_REG_MODE         (0x00)  // Device mode, either WORKING or FACTORY
#define FT6206_VAL_WORKMODE     (0x00)  // Work mode
#define FT6206_VAL_FACTORYMODE  (0x40)  // Factory mode

#define FT6206_REG_STATUS       (0x02)  // Status (touched?)
#define FT6206_REG_X_HI         (0x03)  // X High bits and flags
#define FT6206_REG_X_LO         (0x04)  // X Low bits
#define FT6206_REG_Y_HI         (0x05)  // Y High bits and flags
#define FT6206_REG_Y_LO         (0x06)  // Y Low bits

#define FT6206_REG_THRESHHOLD   (0x80)  // Threshold for touch detection
#define FT6206_REG_POINTRATE    (0x88)  // Point rate
#define FT6206_REG_FIRMVERS     (0xA6)  // Firmware version
#define FT6206_REG_CHIPID       (0xA3)  // Chip selecting
#define FT6206_REG_VENDID       (0xA8)  // FocalTech's panel ID

#define FT6206_VAL_VENDID       (0x11)  // FocalTech's panel ID
#define FT6206_VAL_CHIPID       (0x06)  // Chip selecting

// calibrated for Adafruit 2.8" Cap Touch Screen
#define FT6206_DEFAULT_THRESHOLD  (128) // Default threshold for touch detection

/* Global variables */
extern uint8_t TS_rotation;    // Display rotation (0 thru 3)
extern uint16_t TS_width;       // Display width as modified by current rotation
extern uint16_t TS_height;     // Display height as modified by current rotation

extern uint8_t TS_isTouched;    // non-zero if touch screen is being touched
extern uint16_t TS_touchX;  // x coordinate of current touch after accounting for current rotation
extern uint16_t TS_touchY;  // y coordinate of current touch after accounting for current rotation

/* Function prototypes */
int8_t TS_Init( uint8_t thresh );
void TS_ReadData( void );
void TS_SetRotation( uint8_t m );

#endif // _TS_H
