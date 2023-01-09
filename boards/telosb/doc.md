/**
@defgroup    boards_telosb TelosB
@ingroup     boards
@brief       Support for the TelosB board

## MCU

| MCU        | TI MSP430F1611    |
|:------------- |:--------------------- |
| Family | MSP430    |
| Vendor | Texas Instruments |
| Package       | 64 QFN |
| RAM        | 10Kb  |
| Flash      | 48Kb          |
| Frequency  | 8MHz |
| FPU        | no                |
| Timers | 2 (2x 16bit)  |
| ADCs       | 1x 8 channel 12-bit           |
| UARTs      | 2                 |
| SPIs       | 2 |
| I2Cs       | 1     |
| Vcc        | 2.0V - 3.6V           |
| Datasheet / Reference Manual   | [Datasheet](https://www.ti.com/product/MSP430F1611) |
| User Guide | [User Guide](https://www.ti.com/lit/ug/slau049f/slau049f.pdf)|

## Radio

| RF Chip           | Texas Instruments® CC2420 |
|:-------------------- |:------------------------- |
| Frequency Band       | 2.4GHz ~ 2.485GHz |
| Standard compliance  | IEEE 802.15.4 compliant |
| Receive Sensitivity  | -95dBm typ |
| Transfer Rate         | 250Kbps |
| RF Power          | -25dBm ~ 0dBm   |
| Range                 | ~120m(outdoor), 20~30m(indoor) |
| Current Draw          | RX: 18.8mA TX: 17.4mA Sleep mode: 1uA |
| RF Power Supply      | 2.1V ~ 3.6V |
| Antenna           | Dipole Antenna / PCB Antenna    |
| Datasheet            | [Datasheet](http://www.ti.com.cn/general/cn/docs/lit/getliterature.tsp?genericPartNumber=cc2420&fileType=pdf) |

## Flashing RIOT

Flashing RIOT on the TelosB is quite straight forward, just connect your
board using the USB connector to your host computer and type:

`make flash`

This should take care of everything!

## Using the shell

The shell is using the UART interface of the TelosB at 115200 baud.

## More information

[MEMSIC](http://www.memsic.com/userfiles/files/DataSheets/WSN/telosb_datasheet.pdf)
 */
