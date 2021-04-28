/**
 * @file lcd.c
 * @brief   Header file for driver for the ILI9341 2.8" TFT LCD Display (Adafruit # 2090)
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

#ifndef _LCD_H
#define _LCD_H

#define LCD_WIDTH       240     // ILI9341 max TFT width
#define LCD_HEIGHT      320     // ILI9341 max TFT height

#define ILI9341_NOP     0x00    // No-op register
#define ILI9341_SWRESET 0x01    // Software reset register
#define ILI9341_RDDID   0x04    // Read display identification information
#define ILI9341_RDDST   0x09    // Read Display Status

#define ILI9341_SLPIN   0x10    // Enter Sleep Mode
#define ILI9341_SLPOUT  0x11    // Sleep Out
#define ILI9341_PTLON   0x12    // Partial Mode ON
#define ILI9341_NORON   0x13    // Normal Display Mode ON

#define ILI9341_RDMODE  0x0A    // Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   // Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   // Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   // Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F // Read Display Self-Diagnostic Result

#define ILI9341_INVOFF  0x20    // Display Inversion OFF
#define ILI9341_INVON   0x21    // Display Inversion ON
#define ILI9341_GAMMASET 0x26   // Gamma Set
#define ILI9341_DISPOFF 0x28    // Display OFF
#define ILI9341_DISPON  0x29    // Display ON

#define ILI9341_CASET   0x2A    // Column Address Set
#define ILI9341_PASET   0x2B    // Page Address Set
#define ILI9341_RAMWR   0x2C    // Memory Write
#define ILI9341_RAMRD   0x2E    // Memory Read

#define ILI9341_PTLAR   0x30    // Partial Area
#define ILI9341_VSCRDEF 0x33    // Vertical Scrolling Definition
#define ILI9341_MADCTL  0x36    // Memory Access Control
#define ILI9341_VSCRSADD 0x37   // Vertical Scrolling Start Address
#define ILI9341_PIXFMT  0x3A    // COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 0xB1    // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2    // Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3    // Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR  0xB4    // Display Inversion Control
#define ILI9341_DFUNCTR 0xB6    // Display Function Control

#define ILI9341_PWCTR1 0xC0     // Power Control 1
#define ILI9341_PWCTR2 0xC1     // Power Control 2
#define ILI9341_PWCTR3 0xC2     // Power Control 3
#define ILI9341_PWCTR4 0xC3     // Power Control 4
#define ILI9341_PWCTR5 0xC4     // Power Control 5
#define ILI9341_VMCTR1 0xC5     // VCOM Control 1
#define ILI9341_VMCTR2 0xC7     // VCOM Control 2

#define ILI9341_RDID1 0xDA      // Read ID 1
#define ILI9341_RDID2 0xDB      // Read ID 2
#define ILI9341_RDID3 0xDC      // Read ID 3
#define ILI9341_RDID4 0xDD      // Read ID 4

#define ILI9341_GMCTRP1 0xE0    // Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1    // Negative Gamma Correction
//#define ILI9341_PWCTR6     0xFC

// Color definitions
#define LCD_BLACK       0x0000  //   0,   0,   0
#define LCD_NAVY        0x000F  //   0,   0, 123
#define LCD_DARKGREEN   0x03E0  //   0, 125,   0
#define LCD_DARKCYAN    0x03EF  //   0, 125, 123
#define LCD_MAROON      0x7800  // 123,   0,   0
#define LCD_PURPLE      0x780F  // 123,   0, 123
#define LCD_OLIVE       0x7BE0  // 123, 125,   0
#define LCD_LIGHTGREY   0xC618  // 198, 195, 198
#define LCD_DARKGREY    0x7BEF  // 123, 125, 123
#define LCD_BLUE        0x001F  //   0,   0, 255
#define LCD_GREEN       0x07E0  //   0, 255,   0
#define LCD_CYAN        0x07FF  //   0, 255, 255
#define LCD_RED         0xF800  // 255,   0,   0
#define LCD_MAGENTA     0xF81F  // 255,   0, 255
#define LCD_YELLOW      0xFFE0  // 255, 255,   0
#define LCD_WHITE       0xFFFF  // 255, 255, 255
#define LCD_ORANGE      0xFD20  // 255, 165,   0
#define LCD_GREENYELLOW 0xAFE5  // 173, 255,  41
#define LCD_PINK        0xFC18  // 255, 130, 198

/**
 * Font data stored PER GLYPH
 */
typedef struct
{
    uint16_t bitmapOffset;  // Pointer into GFXfont->bitmap
    uint8_t width;        // Bitmap dimensions in pixels
    uint8_t height;       // Bitmap dimensions in pixels
    uint8_t xAdvance;     // Distance to advance cursor (x axis)
    int8_t xOffset;      // X dist from cursor pos to UL corner
    int8_t yOffset;      // Y dist from cursor pos to UL corner
} GFXglyph;

/**
 * Data stored for FONT AS A WHOLE
 */
typedef struct
{
    uint8_t *bitmap;    // Glyph bitmaps, concatenated
    GFXglyph *glyph;     // Glyph array
    uint16_t first;     // ASCII extents (first char)
    uint16_t last;      // ASCII extents (last char)
    uint8_t yAdvance;  // Newline distance (y axis)
} GFXfont;

/**
 * Pointer to font data structure
 */
extern const GFXfont *LCD_font;
extern const GFXfont FreeMono12pt7b;
extern const GFXfont FreeSans12pt7b;
extern const GFXfont FreeSerif12pt7b;

extern uint8_t LCD_rotation;    // Display rotation (0 thru 3)
extern uint16_t LCD_width;      // Display width as modified by current rotation
extern uint16_t LCD_height;    // Display height as modified by current rotation

extern int16_t LCD_cursor_x;       // x location to start print()ing text
extern int16_t LCD_cursor_y;       // y location to start print()ing text
extern uint16_t LCD_textcolor;     // 16-bit background color for print()
extern uint16_t LCD_textbgcolor;   // 16-bit text color for print()
extern uint8_t LCD_textsize_x;  // Desired magnification in X-axis of text to print()
extern uint8_t LCD_textsize_y;  // Desired magnification in Y-axis of text to print()
extern uint8_t LCD_wrap;         // If set, 'wrap' text at right edge of display

/**
 * Public Method Definitions
 */

void LCD_Init( void );
void LCD_SetRotation( uint8_t m );
void LCD_InvertDisplay( uint8_t invert );
void LCD_ScrollTo( uint16_t y );
void LCD_SetScrollMargins( uint16_t top, uint16_t bottom );
void LCD_SetAddrWindow( uint16_t x1, uint16_t y1, uint16_t w, uint16_t h );
void LCD_WriteColor( uint16_t color, uint32_t len );
void LCD_WriteFillRectPreclipped( int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color );
void LCD_DrawPixel( int16_t x, int16_t y, uint16_t color );
void LCD_DrawFillRect( int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color );
void LCD_DrawHLine( int16_t x, int16_t y, int16_t w, uint16_t color );
void LCD_DrawVLine( int16_t x, int16_t y, int16_t h, uint16_t color );
void LCD_DrawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color );
void LCD_DrawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
void LCD_FillScreen( uint16_t color );

void LCD_SetTextSize( uint8_t size_x, uint8_t size_y );
void LCD_SetCursor( uint8_t curs_x, uint8_t curs_y );
void LCD_SetTextColor( uint16_t color );
void LCD_WriteChar( int16_t x, int16_t y, unsigned char c, uint16_t color,
        uint8_t size_x, uint8_t size_y );
void LCD_DrawChar( uint8_t c );
void LCD_DrawText( const uint8_t *text );

#endif // _LCD_H
