# nucleo_F030R8_TFT

Tinkering with a **2.8" TFT LCD with Cap Touch Breakout Board w/MicroSD Socket** attached to an 
STM32 **Nucleo-F030R8** board. The **STM32F0303R8** connects via SPI1 port to the **ILI9341** LCD driver and 
via I2C1 port to the **FT6206** capacitive touch screen contoller.

## Requirements
 - "Nucleo-F030R9" Developement Board from [ST Microelectronics](https://www.st.com/)
 - "2.8" TFT LCD with Cap Touch Breakout Board w/MicroSD Socket" from Adafruit.com [Product ID: 2090](https://www.adafruit.com/product/2090)
 - "STM32CubeIDE", an Eclipse-based IDE from ST Microelectronics.
  - Serial Port Terminal (PuTTY, TeraTerm, microcom or equivalent). 

## Setup

See the spreadsheet in the Docs folder and the photo on the Adafruit website for a visual aid. 
External connections to the Nucleo Board:

### Power
 - Connect Nucleo CN6 Pin 5 (**5V**) to TFT **Vin** pin.
 - Connect Nucleo CN5 Pin 7 (**GND**) to TFT **GND** pin.
 
### SPI Port
 - On the TFT board, use solder to close the three jumpers marked **IM1**, **IM2**, and **IM3**. Leave **IM0** open. This enables SPI mode instead of parallel mode.
 - (Optional) On the Nucle-F030R8, remove **SB21** to disconnect the Green User LED from PA5, used as the SPI Clock.
 - Connect Nucleo CN5 Pin 6 (**PA5**, Arduino D13) to TFT **CLK** pin.
 - Connect Nucleo CN5 Pin 5 (**PA6**, Arduino D12) to TFT **MISO** pin.
 - Connect Nucleo CN5 Pin 4 (**PA7**, Arduino D11) to TFT **MOSI** pin.
 - Connect Nucleo CN5 Pin 3 (**PB6**, Arduino D10) to TFT **CS** pin.
 - Connect Nucleo CN5 Pin 2 (**PC7**, Arduino D9) to TFT **D/C** pin. 

### I2C Port
 - Connect Nucleo CN5 Pin 10 (**PB8**, SCL) to TFT **SCL** pin.
 - Connect Nucleo CN5 Pin 9 (**PB9**, SDA) to TFT **SDA** pin.
  
### ST-LINK 
 - ST-LINK serial port is set to 38,400 baud, 8 bits, no parity, 1 stop bit (38.4K, 8N1)
   
## Usage
- TBD

## License:

[MIT License](../master/LICENSE)