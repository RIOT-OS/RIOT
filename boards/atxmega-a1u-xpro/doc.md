/**
@defgroup    boards_atxmega-a1u-xpro ATxmega-A1U Xplained Pro board
@ingroup     boards
@brief       Support for the ATxmega-A1U Xplained Pro board

## Overview

The ATxmega-A1U Xplained Pro is the reference to develop with XMEGA's.

### MCU
| MCU           | ATxmega128A1U                                 |
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
| USB           | 1 port                                        |
| DMA           | 4 Channels                                    |
| Event System  | 8 Channels                                    |
| Ext. INT      | All GPIOs                                     |
| Crypto        | AES/DES, CRC-16, CRC-32                       |
| Vcc           | 1.6V - 3.6V                                   |
| Datasheet     | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATxmega128A1U-64A1U-Data-Sheet-DS40002058A.pdf) |
| Xmega Manual  | [Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf) |
| Guide         | [User Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42211-XMEGA-A1U-Xplained-Pro_User-Guide.pdf) |
| Design Files  | [Files](https://ww1.microchip.com/downloads/en/DeviceDoc/XMEGA-A1U-Xplained-Pro_Design-Documentation.zip) |

## Flashing the Device

The ATxmega-A1U Xplained Pro came with a full EDBG + CDC-ACM. The EDBG works
with default avrdude tool. However, the below programmer must have at
/etc/avrdude.conf file:

programmer
  id    = "xplainedpro_pdi";
  desc  = "Atmel AVR XplainedPro in PDI mode";
  type  = "jtagice3_pdi";
  connection_type = usb;
  usbpid = 0x2111;
;

In order to flash the ATxmega128A1U, simple run:

    make BOARD=atxmega-a1u-xpro flash

## Serial Terminal

The CDC-ACM will enumerate a /dev/ttyACM device.

    make BOARD=atxmega-a1u-xpro term

## On-Chip Debugging (OCD)

    make debug

 */
