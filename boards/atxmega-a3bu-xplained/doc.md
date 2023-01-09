/**
@defgroup    boards_atxmega-a3bu-xplained ATxmega-A3BU Xplained board
@ingroup     boards
@brief       Support for the ATxmega-A3BU Xplained board

The ATxmega-A3BU Xplained is a reference to develop with XMEGA's.

### MCU
| MCU           | ATxmega256A3BU                                |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATxmega                                   |
| Vendor        | Microchip (previously Atmel)                  |
| Flash         | 256KiB                                        |
| RAM           | 16KiB                                         |
| EEPROM        | 4KiB                                          |
| Frequency     | up to 32MHz                                   |
| Timers        | 7 16bit (32 bit combining 2 x 16 bit)         |
| ACs           | 4 Analog Comparators                          |
| ADCs          | 2 - 16 channels - 12 bit - 2msps              |
| ADCs          | 1 - 2 channels - 12 bit - 1msps               |
| UARTs         | 6 (can be used in SPI mode) with 1 IrDA       |
| SPIs          | 2                                             |
| I2Cs          | 2 (called TWI)                                |
| USB           | 1 port                                        |
| DMA           | 4 Channels                                    |
| Event System  | 8 Channels                                    |
| Ext. INT      | All GPIOs                                     |
| Crypto        | AES/DES, CRC-16, CRC-32                       |
| Vcc           | 1.6V - 3.6V                                   |
| Datasheet     | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8362-8-and-16bit-AVR-microcontroller-ATxmega256A3BU_datasheet.pdf) |
| Xmega Manual  | [Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf) |
| Guide         | [AVR1923 PDF](http://ww1.microchip.com/downloads/en/Appnotes/doc8394.pdf) |
| Schematic     | [AVR1923 ZIP](https://ww1.microchip.com/downloads/en/AppNotes/AVR1923.zip) |

## Flashing the Device

The ATxmega-A3BU Xplained needs an external programmer like atmelice.

In order to flash the ATxmega256A3BU, simple run:

    make BOARD=atxmega-a3bu-xplained flash

## Serial Terminal

This board needs a serial converter. The default STDIO is at J1 pins 2/3.

    make BOARD=atxmega-a3bu-xplained term

 */
