/*!
 * @file FT6206.h
 */

#ifndef _FT6206_H
#define _FT6206_H

#define FT6206_WIDTH            (240)   // Touch Screen Width
#define FT6206_HEIGHT           (320)   // Touch Screen Height

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
extern uint8_t  TS_rotation;    // Display rotation (0 thru 3)
extern uint16_t TS_width;       // Display width as modified by current rotation
extern uint16_t TS_height;      // Display height as modified by current rotation

extern uint8_t FT6206_touched;
extern uint16_t FT6206_touchX;
extern uint16_t FT6206_touchY;

/* Function prototypes */
int8_t FT6206_init(uint8_t thresh);
void FT6206_readData(void);
void FT6206_setRotation(uint8_t m);

#endif // _FT6206_H
