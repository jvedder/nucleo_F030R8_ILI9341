/**
 * @file lcd.c
 *
 */
#include "main.h"
#include "spi.h"
#include "lcd.h"
#include <stdlib.h>


#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

#define CS_ACTIVE()  	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define CS_IDLE()	    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)

#define DC_COMMAND()	HAL_GPIO_WritePin(DATA_CMD_GPIO_Port, DATA_CMD_Pin, GPIO_PIN_RESET)
#define DC_DATA()	    HAL_GPIO_WritePin(DATA_CMD_GPIO_Port, DATA_CMD_Pin, GPIO_PIN_SET)

#define LCD_TIMEOUT		1000	// 1 second

#define SWAP_INT16(a, b) {int16_t t = a; a=b; b=t;}                                                             \


uint8_t  LCD_rotation;    // Display rotation (0 thru 3)
uint16_t LCD_width;       // Display width as modified by current rotation
uint16_t LCD_height;      // Display height as modified by current rotation


///**
// *  Sends a single Command byte without any data
// * @param   commandByte       The Command Byte
// */
//
//void sendCommand( uint8_t commandByte )
//{
//    LCD_CS_LOW( );  // Start Transaction
//    LCD_DC_LOW( );  // Command mode
//    HAL_SPI_Transmit( &hspi1, &commandByte, 1, LCD_TIMEOUT );
//    LCD_DC_HIGH( ); // Data Mode
//    LCD_CS_HIGH();  // End Transaction
//}

/**
 * Send Command handles command with data bytes.
 *
 * @param   commandByte       The Command Byte
 * @param   dataBytes         A pointer to the Data bytes to send
 * @param   numDataBytes      The number of bytes we should send
 */
static void sendCommand( uint8_t commandByte, uint8_t *dataBytes, uint8_t numDataBytes )
{
    CS_ACTIVE( );	// Start Transaction
    DC_COMMAND( );	// Command mode
    HAL_SPI_Transmit( &hspi1, &commandByte, 1, LCD_TIMEOUT );
    DC_DATA( );	// Data Mode
    if ( numDataBytes > 0 )
    {
        HAL_SPI_Transmit( &hspi1, dataBytes, numDataBytes, LCD_TIMEOUT );
    }
    CS_IDLE( );  // End Transaction
}

// clang-format off
static const uint8_t initcmd[] =
{
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control , Rotation 0
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};
// clang-format on

void LCD_Init( void )
{
    sendCommand( ILI9341_SWRESET, NULL, 0 );  // Engage software reset
    HAL_Delay( 150 );

    uint8_t cmd;
    uint8_t x;
    uint8_t numArgs;
    const uint8_t *addr = initcmd;

    while ( ( cmd = (uint8_t)(*addr++) ) > 0 )
    {
        x = *addr++ ;
        numArgs = x & 0x7F;
        sendCommand( cmd, (uint8_t *)addr, numArgs );
        addr += numArgs;
        if ( x & 0x80 ) HAL_Delay( 150 );
    }

    LCD_width = ILI9341_TFTWIDTH;
    LCD_height = ILI9341_TFTHEIGHT;
}

/**
 * Set origin of (0,0) and orientation of TFT display.
 *
 * @param   m  The index for rotation, from 0-3 inclusive
 */
void LCD_SetRotation(uint8_t m) {
	LCD_rotation = m % 4; // can't be higher than 3
	switch (LCD_rotation) {
	case 0:
		m = (MADCTL_MX | MADCTL_BGR);
		LCD_width = ILI9341_TFTWIDTH;
		LCD_height = ILI9341_TFTHEIGHT;
		break;
	case 1:
		m = (MADCTL_MV | MADCTL_BGR);
		LCD_width = ILI9341_TFTHEIGHT;
		LCD_height = ILI9341_TFTWIDTH;
		break;
	case 2:
		m = (MADCTL_MY | MADCTL_BGR);
		LCD_width = ILI9341_TFTWIDTH;
		LCD_height = ILI9341_TFTHEIGHT;
		break;
	case 3:
		m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		LCD_width = ILI9341_TFTHEIGHT;
		LCD_height = ILI9341_TFTWIDTH;
		break;
	}

	sendCommand(ILI9341_MADCTL, &m, 1);
}

/**
 *   Enable/Disable display color inversion.
 *
 *   @param   invert True to invert, False to have normal color
 */
void LCD_InvertDisplay(uint8_t invert)
{
    sendCommand(invert ? ILI9341_INVON : ILI9341_INVOFF, NULL, 0);
}

/**
 *  Scroll display memory.
 *
 *   @param   y How many pixels to scroll display by
 */
void LCD_ScrollTo( uint16_t y )
{
    uint8_t data[2];
    data[0] = y >> 8;
    data[1] = y & 0xff;
    sendCommand( ILI9341_VSCRSADD, (uint8_t*) data, 2 );
}

/**
 * Set the height of the Top and Bottom Scroll Margins
 *
 * @param   top The height of the Top scroll margin
 * @param   bottom The height of the Bottom scroll margin
 */
void LCD_SetScrollMargins( uint16_t top, uint16_t bottom )
{
    // TFA+VSA+BFA must equal 320
    if ( top + bottom <= ILI9341_TFTHEIGHT )
    {
        uint16_t middle = ILI9341_TFTHEIGHT - top + bottom;
        uint8_t data[6];
        data[0] = top >> 8;
        data[1] = top & 0xff;
        data[2] = middle >> 8;
        data[3] = middle & 0xff;
        data[4] = bottom >> 8;
        data[5] = bottom & 0xff;
        sendCommand( ILI9341_VSCRDEF, (uint8_t*) data, 6 );
    }
}

/**
 * Converts a 24-bit RGB (8 bits each) to a 16-bit RGB "565" color.
 */
uint16_t LCD_Color565( uint8_t r, uint8_t g, uint8_t b )
{
    return ( ( r & 0xF8 ) << 8 ) | ( ( g & 0xFC ) << 3 ) | ( b >> 3 );
}

/**
 * -------------------------------------------------------------------------
 * The LCD_Write* functions are lower-level and care must be taken on their
 * sequence.
 * TODO:Maybe make te LCD_Write* functions static.
 * -------------------------------------------------------------------------
 */

/**
 *   Set the "address window" - the rectangle we will write to RAM with
 *   the next chunk of SPI data writes. The ILI9341 will automatically wrap
 *   the data as each row is filled.
 *
 *   @param   x1  TFT memory 'x' origin
 *   @param   y1  TFT memory 'y' origin
 *   @param   w   Width of rectangle
 *   @param   h   Height of rectangle
 */
void LCD_SetAddrWindow( uint16_t x1, uint16_t y1, uint16_t w, uint16_t h )
{
    /**
     *  Note: The ILI9341 receives 16-but data high byte first, but STM32F0 is little
     *  endian (low byte first). So we have to use a byte array to get the bytes in
     *  the correct order
     */
    uint8_t buffer[4];
    uint16_t end;

    end = x1 + w - 1 ;
    buffer[0] = x1 >> 8;
    buffer[1] = x1;
    buffer[2] = end >> 8;
    buffer[3] = end;
    sendCommand( ILI9341_CASET, buffer, 4);  // Column address set

    end = y1 + h - 1;
    buffer[0] = y1 >> 8;
    buffer[1] = y1;
    buffer[2] = end >> 8;
    buffer[3] = end;
    sendCommand( ILI9341_PASET, buffer, 4);  // Row address set

    //sendCommand( ILI9341_RAMWR, NULL,0 );  // Write to RAM
}

/**
 * Block fill operation for fillScreen, fillRect, H/V line, etc.
 * Requires LCD_SetAddrWindow() has previously been called to set the fill
 * bounds.  'len' is inclusive, MUST be >= 1.
 */
void LCD_WriteColor(uint16_t color, uint32_t len)
{
  uint8_t buffer[2];
  uint8_t cmd = ILI9341_RAMWR;

  buffer[0] = color >>8;
  buffer[1] = color;

  CS_ACTIVE( );    // Start Transaction
  DC_COMMAND( );  // Command mode
  HAL_SPI_Transmit( &hspi1, &cmd, 1, LCD_TIMEOUT );
  DC_DATA( ); // Data Mode
  while (len--)
  {
      HAL_SPI_Transmit( &hspi1, buffer, 2, LCD_TIMEOUT );
  }
  CS_IDLE( );  // End Transaction
}

/**
 *  Fills a rectangle on the display with a solid color.
 *
 *  Clip bounds are NOT checked.
 *
 *  @param  x      Horizontal position of first corner.
 *  @param  y      Vertical position of first corner.
 *  @param  w      Rectangle width in pixels (positive = right of first
 *                 corner, negative = left of first corner).
 *  @param  h      Rectangle height in pixels (positive = below first
 *                 corner, negative = above first corner).
 *  @param  color  16-bit fill color in '565' RGB format.
 */
void LCD_WriteFillRectPreclipped( int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color )
{
    LCD_SetAddrWindow( x, y, w, h );
    LCD_WriteColor( color, (uint32_t) w * h );
}

/**
 *   Write a line using Bresenham's algorithm.
 *
 *   Clip bounds are NOT checked.
 *
 *   @param    x0  Start point x coordinate
 *   @param    y0  Start point y coordinate
 *   @param    x1  End point x coordinate
 *   @param    y1  End point y coordinate
 *   @param    color 16-bit 5-6-5 Color to draw with
 */
void LCD_WriteLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color )
{
    int16_t steep = abs( y1 - y0 ) > abs( x1 - x0 );
    if ( steep )
    {
        SWAP_INT16( x0, y0 );
        SWAP_INT16( x1, y1 );
    }

    if ( x0 > x1 )
    {
        SWAP_INT16( x0, x1 );
        SWAP_INT16( y0, y1 );
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs( y1 - y0 );

    int16_t err = dx / 2;
    int16_t ystep;

    if ( y0 < y1 )
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for ( ; x0 <= x1; x0++ )
    {
        if ( steep )
        {
            LCD_DrawPixel( y0, x0, color );
        }
        else
        {
            LCD_DrawPixel( x0, y0, color );
        }
        err -= dy;
        if ( err < 0 )
        {
            y0 += ystep;
            err += dx;
        }
    }
}


/**
 * -------------------------------------------------------------------------
 * The LCD_Draw* functions are higher level and include screen clipping,
 * negative height or width, and off-screen detection
 * -------------------------------------------------------------------------
 */

/**
 * Draw a single pixel on the display at the specified coordinates.
 * Edge clipping is performed here.
 *
 *    @param  x      Horizontal position (0 = left).
 *    @param  y      Vertical position   (0 = top).
 *    @param  color  16-bit pixel color in '565' RGB format.
 */
void LCD_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x >= 0) && (x < LCD_width) && (y >= 0) && (y < LCD_height))
  {
    LCD_SetAddrWindow(x, y, 1, 1);
    LCD_WriteColor(color, 1);
  }
}

/**
 * Draw a filled rectangle to the display. Edge clipping, off screen, and
 * negative height or width is gracefully handled.
 *
 *  @param  x      Horizontal position of first corner.
 *  @param  y      Vertical position of first corner.
 *  @param  w      Rectangle width in pixels (positive = right of first
 *                 corner, negative = left of first corner).
 *  @param  h      Rectangle height in pixels (positive = below first
 *                 corner, negative = above first corner).
 *  @param  color  16-bit fill color in '565' RGB format.
 */
void LCD_DrawFillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color )
{

    // Zero height or width does not display
    if ( w == 0 || h == 0 ) return;

    // If negative width...
    if ( w < 0 )
    {
        x += w + 1; // Move X to left edge
        w = -w;     // Use positive width
    }

    // If negative height...
    if ( h < 0 )
    {
        y += h + 1; // Move Y to top edge
        h = -h;     // Use positive height
    }

    // compute right, bottom coordinate
    int16_t x2 = x + w - 1;
    int16_t y2 = y + h - 1;

    // Completely Off screen does not display
    if ( x > LCD_width  || x2 < 0 ||  y > LCD_height  || y2 < 0 ) return;

    // Clip left
    if ( x < 0 )
    {
        x = 0;
        w = x2 + 1;
    }
    // Clip top
   if ( y < 0 )
    {
        y = 0;
        h = y2 + 1;
    }
    //clip right
   if ( x2 >= LCD_width )
    {
        w = LCD_width - x;
    }
    //clip bottom
    if ( y2 >= LCD_height )
    {
        h = LCD_height - y;
    }

    LCD_WriteFillRectPreclipped( x, y, w, h, color );
}


/**
 * Draw a horizontal line on the display. Edge clipping and rejection
 * is performed here.
 *
 *   @param  x      Horizontal position of first point.
 *   @param  y      Vertical position of first point.
 *   @param  w      Line width in pixels (positive = right of first point,
 *                  negative = point of first corner).
 *   @param  color  16-bit line color in '565' RGB format.
 */
void LCD_DrawHLine( int16_t x, int16_t y, int16_t w, uint16_t color )
{
    LCD_DrawFillRect( x, y, w, 1, color );
}

/**
 * Draw a horizontal line on the display. Edge clipping and rejection
 * is performed.
 *
 *   @param  x      Horizontal position of first point.
 *   @param  y      Vertical position of first point.
 *   @param  h      Line height in pixels (positive = below first point,
 *                  negative = above first point).
 *   @param  color  16-bit line color in '565' RGB format.
 */
void LCD_DrawVLine( int16_t x, int16_t y, int16_t h, uint16_t color )
{
    LCD_DrawFillRect( x, y, 1, h, color );
}

/**
 *  Draw a line. Edge clipping and rejection
 * is performed.
 *
 *   @param    x0  Start point x coordinate
 *   @param    y0  Start point y coordinate
 *   @param    x1  End point x coordinate
 *   @param    y1  End point y coordinate
 *   @param    color 16-bit 5-6-5 Color to draw with
 */
void LCD_DrawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color )
{
    if ( x0 == x1 )
    {
        LCD_DrawVLine( x0, y0, y1 - y0 + 1, color );
    }
    else if ( y0 == y1 )
    {
        LCD_DrawHLine( x0, y0, x1 - x0 + 1, color );
    }
    else
    {
        LCD_WriteLine( x0, y0, x1, y1, color );
    }
}

/**
 *  Draw a rectangle with no fill color. Edge clipping and rejection
 *  is performed.
 *
 *  @param    x   Top left corner x coordinate
 *  @param    y   Top left corner y coordinate
 *  @param    w   Width in pixels
 *  @param    h   Height in pixels
 *  @param    color 16-bit 5-6-5 Color to draw with
 */
/**************************************************************************/
void LCD_DrawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color )
{
    LCD_DrawHLine( x, y, w, color );
    LCD_DrawHLine( x, y + h - 1, w, color );
    LCD_DrawVLine( x, y, h, color );
    LCD_DrawVLine( x + w - 1, y, h, color );
}

/**
 * Fills the screen with a solid color
 *
 *  @param  color  16-bit line color in '565' RGB format.
 */
void LCD_FillScreen( uint16_t color )
{
    LCD_DrawFillRect( 0, 0, LCD_width, LCD_height, color );
}
