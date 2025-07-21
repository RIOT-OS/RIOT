@defgroup    boards_atxmega-a1-xplained ATxmega-A1 Xplained board
@ingroup     boards
@brief       Support for the ATxmega-A1 Xplained board

## Overview

The ATxmega-A1 Xplained is an old reference to develop with XMEGA's.

### MCU
| MCU           | ATxmega128A1                                  |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATxmega                                   |
| Vendor        | Microchip (previously Atmel)                  |
| Flash         | 128KiB                                        |
| RAM           | 8KiB                                          |
| EBI           | 16MiB SRAM, 128MiB SDRAM                      |
| EEPROM        | 2KiB                                          |
| Frequency     | up to 32MHz                                   |
| Timers        | 8 16bit (32 bit combining 2 x 16 bit)         |
| ACs           | 4 Analog Comparators                          |
| ADCs          | 2 - 16 channels - 12 bit - 2msps              |
| ADCs          | 2 - 2 channels - 12 bit - 1msps               |
| UARTs         | 8 (can be used in SPI mode) with 1 IrDA       |
| SPIs          | 4                                             |
| I2Cs          | 4 (called TWI)                                |
| DMA           | 4 Channels                                    |
| Event System  | 8 Channels                                    |
| Ext. INT      | All GPIOs                                     |
| Crypto        | AES/DES, CRC-16, CRC-32                       |
| Vcc           | 2.7V - 5.5V (when clocked at 8MHz)            |
| Datasheet     | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATxmega128A1U-64A1U-Data-Sheet-DS40002058A.pdf) |
| Xmega Manual  | [Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf) |
| Guide         | [AVR1924 PDF](http://ww1.microchip.com/downloads/en/AppNotes/doc8370.pdf) |
| Schematic     | [AVR1924 ZIP](https://ww1.microchip.com/downloads/en/AppNotes/AVR1924.zip) |

## Flashing the Device

The ATxmega-A1 Xplained needs an external programmer like atmelice.

In order to flash the ATxmega128A1, simple run:

    make BOARD=atxmega-a1-xplained flash

## Serial Terminal

The CDC-ACM will enumerate a /dev/ttyACM device. The STDIO at CDC-ACM shares
with J4 pins 2/3.

    make BOARD=atxmega-a1-xplained term
