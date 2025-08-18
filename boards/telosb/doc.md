@defgroup   boards_telosb TelosB
@ingroup    boards
@brief      Support for the TelosB board

## MCU

| MCU               | TI MSP430F1611                                                |
|:----------------- |:------------------------------------------------------------- |
| Family            | MSP430                                                        |
| Vendor            | Texas Instruments                                             |
| Package           | 64 QFN                                                        |
| RAM               | 10 KiB                                                        |
| Flash             | 48 KiB                                                        |
| Frequency         | 8 MHz                                                         |
| FPU               | no                                                            |
| Timers            | 2 (2 × 16 bit)                                                |
| ADCs              | 1 × 8 channel 12-bit                                          |
| UARTs             | 2                                                             |
| SPIs              | 2                                                             |
| I2Cs              | 1                                                             |
| Vcc               | 2.0V - 3.6V                                                   |
| Datasheet (Board) | [Tmote Sky / TelosB Datasheet](http://web.archive.org/web/20170712171826/http://www.eecs.harvard.edu/~konrad/projects/shimmer/references/tmote-sky-datasheet.pdf) |
| Datasheet (MCU)   | [Datasheet](https://www.ti.com/product/MSP430F1611)           |
| User Guide (MCU)  | [User Guide](https://www.ti.com/lit/ug/slau049f/slau049f.pdf) |

## Radio

| RF Chip               | Texas Instruments® CC2420                 |
|:--------------------- |:----------------------------------------- |
| Frequency Band        | 2.4GHz ~ 2.485GHz                         |
| Standard compliance   | IEEE 802.15.4 compliant                   |
| Receive Sensitivity   | -95 dBm typical                           |
| Transfer Rate         | 250 Kbps                                  |
| RF Power              | -25 dBm ~ 0 dBm                           |
| Range                 | ~120 m (outdoor), 20 ~ 30 m (indoor)      |
| Current Draw          | RX: 18.8 mA TX: 17.4 mA Sleep mode: 1 μA  |
| RF Power Supply       | 2.1 V ~ 3.6 V                             |
| Antenna               | Dipole Antenna / PCB Antenna              |
| Datasheet             | [Datasheet](http://www.ti.com.cn/general/cn/docs/lit/getliterature.tsp?genericPartNumber=cc2420&fileType=pdf) |

## Flashing RIOT

Flashing RIOT on the TelosB is quite straight forward, just connect your
board using the USB connector to your host computer and type:

`make flash`

This should take care of everything!

## Using the shell

The shell is using the UART interface of the TelosB at 9600 baud.

## More information

- [Mote IV Tmote sky (Telos B clone) datasheet (including original Telos B schematics)](http://web.archive.org/web/20170712171826/http://www.eecs.harvard.edu/~konrad/projects/shimmer/references/tmote-sky-datasheet.pdf)
- [MEMSIC Telos B data brief](https://web.archive.org/web/20190215211317/https://www.memsic.com/userfiles/files/DataSheets/WSN/telosb_datasheet.pdf)
