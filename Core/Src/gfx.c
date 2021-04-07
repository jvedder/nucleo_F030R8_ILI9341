/**
 * @file gfx.c
 */

#include "main.h"
#include "lcd.h"
#include "gfx.h"
#define PROGMEM
#include "fonts/FreeMono12pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSerif12pt7b.h"

const GFXfont *GFX_Font = &FreeSerif12pt7b;
int16_t GFX_cursor_x = 0;       // x location to start print()ing text
int16_t GFX_cursor_y = 0;       // y location to start print()ing text
uint16_t GFX_textcolor = 0;     // 16-bit background color for print()
uint16_t GFX_textbgcolor = 0;   // 16-bit text color for print()
uint8_t GFX_textsize_x = 1;  // Desired magnification in X-axis of text to print()
uint8_t GFX_textsize_y = 1;  // Desired magnification in Y-axis of text to print()
uint8_t GFX_wrap = 1;           // If set, 'wrap' text at right edge of display

/**
 * Set text magnification multiplier in x and y.
 *
 * @param  size_x  Desired text width magnification level in X-axis. 1 is default
 * @param  size_y  Desired text width magnification level in Y-axis. 1 is default
 */
void GFX_SetTextSize( uint8_t size_x, uint8_t size_y )
{
    GFX_textsize_x = ( size_x > 0 ) ? size_x : 1;
    GFX_textsize_y = ( size_y > 0 ) ? size_y : 1;
}

/**
 * Set the text cursor position in x and y.
 *
 * @param  curs_x  Desired text position in X-axis.
 * @param  curs_y  Desired text position in Y-axis.
 */
void GFX_SetCursor( uint8_t curs_x, uint8_t curs_y )
{
    GFX_cursor_x = ( curs_x > 0 ) ? curs_x : 1;
    GFX_cursor_y = ( curs_y > 0 ) ? curs_y : 1;
}

/**
 * Set the text cursor position in x and y.
 *
 *  @param    color 16-bit 5-6-5 Color to draw character with.
 */
void GFX_SetTextColor( uint16_t color )
{
    GFX_textcolor = color;
}

/**
 *  Draw a single character. Assumes non-printable and control characters are filtered out.
 *
 *  @param    x   Bottom left corner x coordinate
 *  @param    y   Bottom left corner y coordinate
 *  @param    c   The 8-bit font-indexed (character)
 *  @param    color 16-bit 5-6-5 Color to draw character with
 *  @param    size_x  Font magnification level in X-axis, 1 is 'original' size
 *  @param    size_y  Font magnification level in Y-axis, 1 is 'original' size
 */
void GFX_WriteChar( int16_t x, int16_t y, unsigned char c, uint16_t color,
        uint8_t size_x, uint8_t size_y )
{

    c -= ( GFX_Font->first );
    GFXglyph *glyph = GFX_Font->glyph + c;
    const uint8_t *bitmap = GFX_Font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->xOffset;
    int8_t yo = glyph->yOffset;
    uint8_t xx;
    uint8_t yy;
    uint8_t bits = 0;
    uint8_t bit = 0;
    int16_t xo16 = 0;
    int16_t yo16 = 0;

    if ( size_x > 1 || size_y > 1 )
    {
        xo16 = xo;
        yo16 = yo;
    }

    for ( yy = 0; yy < h; yy++ )
    {
        for ( xx = 0; xx < w; xx++ )
        {
            if ( !( bit++ & 7 ) )
            {
                bits = (uint8_t) ( bitmap[bo++ ] );
            }
            if ( bits & 0x80 )
            {
                if ( size_x == 1 && size_y == 1 )
                {
                    LCD_DrawPixel( x + xo + xx, y + yo + yy, color );
                }
                else
                {
                    LCD_DrawFillRect( x + ( xo16 + xx ) * size_x,
                            y + ( yo16 + yy ) * size_y, size_x, size_y, color );
                }
            }
            bits <<= 1;
        }
    }
}

/**
 *   Draws one character of data. Moves text cursor and supports newline ('\n').
 *
 *   @param  c  The 7-bit ASCII character to draw
 */
void GFX_DrawChar( uint8_t c )
{
    if ( c == '\n' )
    {
        GFX_cursor_x = 0;
        GFX_cursor_y += ( GFX_textsize_y * ( GFX_Font->yAdvance ) );
    }
    else if ( c != '\r' )
    {
        uint8_t first = (uint8_t) ( GFX_Font->first );
        if ( ( first <= c ) && ( c <= GFX_Font->last ) )
        {
            GFXglyph *glyph = GFX_Font->glyph + ( c - first );
            uint8_t w = glyph->width;
            uint8_t h = glyph->height;
            if ( ( w > 0 ) && ( h > 0 ) )
            {  // Is there an associated bitmap?
                int16_t xo = (int8_t) ( glyph->xOffset );
                if ( GFX_wrap
                        && ( ( GFX_cursor_x + GFX_textsize_x * ( xo + w ) )
                                > LCD_width ) )
                {
                    GFX_cursor_x = 0;
                    GFX_cursor_y += ( GFX_textsize_y * ( GFX_Font->yAdvance ) );
                }
                GFX_WriteChar( GFX_cursor_x, GFX_cursor_y, c, GFX_textcolor,
                        GFX_textsize_x, GFX_textsize_y );
            }
            GFX_cursor_x += ( GFX_textsize_x * ( glyph->xAdvance ) );
        }
    }
}


/**
 *   Draws a string of text at the current cursor position. Text must be null-terminated and less than 128
 *   characters. Supports newline ('\n').
 *
 *   @param  text  The null-terminated text string.
 */
void GFX_DrawText( const uint8_t *text )
{
    uint16_t overrun = 128;

    while (*text && overrun)
    {
        /* buffer to remove const attribute */
        uint8_t c = (uint8_t) *text;
        GFX_DrawChar(c);
        text++;
        overrun--;
    }
}

