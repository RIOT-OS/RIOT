@defgroup    boards_cc2538dk CC2538DK
@ingroup     boards
@brief       Support for the Texas Instruments CC2538DK board.

## Overview

The [CC2538DK](http://www.ti.com/tool/cc2538dk) is Texas Instruments'
developer kit for the CC2538 SoC MCU, which combines an ARM Cortex-M3
microcontroller with an IEEE802.15.4 radio.

## Hardware

![cc2538dk](http://www.ti.com/diagrams/cc2538dk_cc2538dk_web_1.jpg)

| MCU               | CC2538SF53                |
|:----------------- |:------------------------- |
| Family            | ARM Cortex-M3             |
| Vendor            | Texas Instruments         |
| RAM               | 32KiB                     |
| Flash             | 512KiB                    |
| Frequency         | 32MHz                     |
| FPU               | no                        |
| Timers            | 4                         |
| ADCs              | 1x 12-bit (8 channels)    |
| UARTs             | 2                         |
| SPIs              | 2                         |
| I2Cs              | 1                         |
| Vcc               | 2V - 3.6V                 |
| Datasheet         | [Datasheet](http://www.ti.com/lit/gpn/cc2538) (pdf file) |
| Reference Manual  | [Reference Manual](http://www.ti.com/lit/pdf/swru319) |

## Flashing and Debugging

By default, RIOT will attempt to flash the MCU via the USB UART using a
Python script named [cc2538-bsl](https://github.com/JelmerT/cc2538-bsl).
Hold down the SELECT pushbutton while pressing RESET to activate the MCU's
internal bootloader, then run:

```shell
BOARD=cc2538dk make flash
```

Activating this bootloader is NOT enabled if the flash content is in factory
default state (e.g. after unboxing). To set the bits in the CCA accordingly you
have to follow the guidelines found [here](https://web.archive.org/web/20170610111337/http://processors.wiki.ti.com/index.php/CC2538_Bootloader_Backdoor).
To manage this first time access you have to download the
["Uniflash"](https://web.archive.org/web/20201215123705/https://processors.wiki.ti.com/index.php/Category:CCS_UniFlash) tool
at TI's website.

Some Linux machines may not recognize the CC2538DK's vendor and product ID
automatically.
If a /dev/ttyUSBx device does not appear, try specifying these codes to the
FTDI driver manually:

`echo 0403 a6d1 > /sys/bus/usb-serial/drivers/ftdi_sio/new_id`

If the path `/sys/bus/usb-serial/drivers/ftdi_sio/` doesn't exist, you also
have to load the module `ftdi_sio` by hand.  Alternatively, you can install a
`udev` rule that configures this on device connection, see [this post on TI's
E2E site](https://e2e.ti.com/support/microcontrollers/c2000/f/171/p/359074/1843485#1843485)
for details.

RIOT will use /dev/ttyUSB1 by default, but if the UART is given a different
device name, you can specity it to RIOT using the PORT variable:

```shell
BOARD=cc2538dk PORT=/dev/ttyUSB2 make flash
```

To flash using a Segger JLink JTAG adapter you need to install Segger's
JLinkExe tool, then specify `PROGRAMMER=jlink` when flashing:

```shell
BOARD=cc2538dk PROGRAMMER=jlink make flash
```
