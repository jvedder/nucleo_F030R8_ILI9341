/**
 * @file gfx.h
 */

#ifndef _GFX_H
#define _GFX_H

/**
 * Font data stored PER GLYPH
 */
typedef struct {
  uint16_t bitmapOffset; // Pointer into GFXfont->bitmap
  uint8_t width;         // Bitmap dimensions in pixels
  uint8_t height;        // Bitmap dimensions in pixels
  uint8_t xAdvance;      // Distance to advance cursor (x axis)
  int8_t xOffset;        // X dist from cursor pos to UL corner
  int8_t yOffset;        // Y dist from cursor pos to UL corner
} GFXglyph;

/**
 * Data stored for FONT AS A WHOLE
 */
typedef struct {
  uint8_t *bitmap;  // Glyph bitmaps, concatenated
  GFXglyph *glyph;  // Glyph array
  uint16_t first;   // ASCII extents (first char)
  uint16_t last;    // ASCII extents (last char)
  uint8_t yAdvance; // Newline distance (y axis)
} GFXfont;

/**
 * Pointer to font data structure
 */
extern const GFXfont *GFX_Font;
extern const GFXfont FreeMono12pt7b;
extern const GFXfont FreeSans12pt7b;
extern const GFXfont FreeSerif12pt7b;

extern int16_t GFX_cursor_x;       // x location to start print()ing text
extern int16_t GFX_cursor_y;       // y location to start print()ing text
extern uint16_t GFX_textcolor;     // 16-bit background color for print()
extern uint16_t GFX_textbgcolor;   // 16-bit text color for print()
extern uint8_t GFX_textsize_x;     // Desired magnification in X-axis of text to print()
extern uint8_t GFX_textsize_y;     // Desired magnification in Y-axis of text to print()
extern uint8_t GFX_wrap;           // If set, 'wrap' text at right edge of display

/**
 * Public function prototypes
 */
void GFX_SetTextSize(uint8_t size_x, uint8_t size_y);
void GFX_SetCursor(uint8_t curs_x, uint8_t curs_y);
void GFX_SetTextColor(uint16_t color);
void GFX_WriteChar( int16_t x, int16_t y, unsigned char c, uint16_t color,
        uint8_t size_x, uint8_t size_y );
void GFX_DrawChar( uint8_t c );


#endif //_GFX_H
