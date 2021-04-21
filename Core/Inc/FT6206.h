/*!
 * @file Adafruit_FT6206.h
 */

#ifndef _FT6206_H
#define _FT6206_H

#define FT6206_ADDR             (0x70)  // I2C address
#define FT6206_G_FT5201ID       (0xA8)  // FocalTech's panel ID
#define FT6206_REG_NUMTOUCHES   (0x02)  // Number of touch points

#define FT6206_NUM_X            (0x33)  // Touch X position
#define FT6206_NUM_Y            (0x34)  // Touch Y position

#define FT6206_REG_MODE         (0x00)  // Device mode, either WORKING or FACTORY
#define FT6206_REG_CALIBRATE    (0x02)  // Calibrate mode
#define FT6206_REG_WORKMODE     (0x00)  // Work mode
#define FT6206_REG_FACTORYMODE  (0x40)  // Factory mode
#define FT6206_REG_THRESHHOLD   (0x80)  // Threshold for touch detection
#define FT6206_REG_POINTRATE    (0x88)  // Point rate
#define FT6206_REG_FIRMVERS     (0xA6)  // Firmware version
#define FT6206_REG_CHIPID       (0xA3)  // Chip selecting
#define FT6206_REG_VENDID       (0xA8)  // FocalTech's panel ID

#define FT6206_VENDID           (0x11)  // FocalTech's panel ID
#define FT6206_CHIPID           (0x06)  // Chip selecting
#define FT6236_CHIPID           (0x36)  // Chip selecting
#define FT6236U_CHIPID          (0x64)  // Chip selecting

// calibrated for Adafruit 2.8" Cap Touch Screen
#define FT6206_DEFAULT_THRESHOLD  (128) // Default threshold for touch detection

/* Global variables */
extern uint8_t FT6206_touched;
extern uint16_t FT6206_touchX;
extern uint16_t FT6206_touchY;

/* Function prototypes */
int8_t FT6206_init(uint8_t thresh);
void FT6206_readData(void);

#endif // _FT6206_H
