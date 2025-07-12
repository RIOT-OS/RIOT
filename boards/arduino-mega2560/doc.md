@defgroup    boards_arduino-mega2560 Arduino Mega 2560
@ingroup     boards
@brief       Support for the Arduino Mega 2560 board

## Overview

The Arduino Mega2560 is one of the larger Arduino boards. It is based on
Atmel's AVR architecture and sports an ATmega2560 MCU. It is like many Arduinos
extensible by using shields.

NOTE: In case you are wondering if flashing RIOT on your Arduino Mega2560
will overwrite the Arduino bootloader: you can be reassured. After flashing RIOT
you can without any intermediate steps just go over to flashing Arduino sketches
again.

## Hardware

![Arduino Mega2560](https://store-cdn.arduino.cc/uni/catalog/product/cache/1/image/500x375/f8876a31b63532bbba4e781c30024a0a/a/0/a000067_front_1_.jpg)

### MCU

| MCU                           | ATmega2560                                |
|:----------------------------- |:----------------------------------------- |
| Family                        | AVR/ATmega                                |
| Vendor                        | Atmel                                     |
| RAM                           | 8 KiB                                     |
| Flash                         | 256 KiB                                   |
| Frequency                     | 16 MHz                                    |
| Timers                        | 6 (2x 8bit, 4x 16bit)                     |
| ADCs                          | 14 analog input pins (10bit resolution)   |
| UARTs                         | 4                                         |
| SPIs                          | 1                                         |
| I2Cs                          | 1 (called TWI)                            |
| Vcc                           | 5.0 V                                     |
| Datasheet / Reference Manual  | [Datasheet and Reference Manual](http://www.atmel.com/images/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf) |
| Board Manual                  | [Board Manual](http://arduino.cc/en/Main/arduinoBoardMega2560) |

Flashing RIOT on the Arduino Mega2560 is quite straight forward, just connect
your Arduino Mega2560 using the programming port to your host computer and type:

`make BOARD=arduino-mega2560 flash`

This should take care of everything!

We use the open `avrdude` tool to write the new code into the ATmega2560's
flash

## Pin Change Interrupts

More pins can be used for hardware interrupts using the Pin Change
Interrupt feature. See @ref boards_common_atmega for details.

## Debugging (WIP)
The ATmega2560 MCU supports JTAG debugging. To use the JTAG debugging on the
Arduino Mega 2560 an external JTAG debugger is required. There are several
options for this MCU/board:
 * [Atmel-ICE](https://www.microchip.com/DevelopmentTools/ProductDetails/atatmel-ice)
   (Note: A version of the Atmel-ICE without case (just the naked but fully
   assembled PCB) is currently the most affordable option.
 * [AVR Dragon](http://www.atmel.com/tools/avrdragon.aspx)
   (Note: This debugger is out of production.)
 * [AVR JTAGICE mkII](http://www.atmel.com/tools/avrjtagicemkii.aspx)
 * [JTAGICE3](http://www.atmel.com/tools/jtagice3.aspx)

There may be other options as well, but I can't comment on how well they
work. I tested debugging RIOT on the Arduino Mega 2560 using an AVR Dragon.

**Important:** To use a JTAG Debugger in conjunction with the ATmega2560 it
is required to change the fuses of the MCU. Additionally it seems to be required
to overwrite the bootloader on the MCU. Because of that it is a necessity to use
an ISP (in system programmer) to do the debugging. This isn't an issue because
all of the afore mentioned devices have ISP capabilities, but it requires some
additional steps to get back normal operation after debugging:
 * flash a new arduino bootloader on the device, e.g. [this one](https://raw.githubusercontent.com/arduino/Arduino-stk500v2-bootloader/master/goodHexFiles/stk500boot_v2_mega2560.hex)
 * restore the fuses to the default state.



### Wiring for the AVR Dragon

In contrast to normal use (USB only), for debugging there are two separate
wiring changes to do:
* connecting the ISP headers
* connecting the JTAG header to the respective pins on the Arduino

Connecting the ISP headers is straight forward: Pin1 on the Dragon connects
to Pin1 on the Arduino Mega2560 and so on.
Connecting the JTAG header needs the following pin mapping:

|AVR Dragon | Arduino Mega 2560|Signal|
|:----------|:-----------------|:-----|
|JTAG1      |A4                |TCK   |
|JTAG2 o. 10|GND               |GND   |
|JTAG3      |A6                |TDO   |
|JTAG4      |+5V               |+5V   |
|JTAG5      |A5                |TMS   |
|JTAG9      |A7                |TDI   |

Ax refers to the analog in pins on the Arduino Mega 2560.
JTAG2 and JTAG10 on the AVR Dragon are both GND, one connection suffices.
All other JTAG Pins are not needed for debugging the Arduino Mega2560

Additional information can be found
[here](http://arduino.cc/en/Hacking/PinMapping2560).

### Fuses
**default:**

| Fuse          | Setting |
|:--------------|:--------|
| Low Fuse      |    0xFF |
| High Fuse     |    0xD8 |
| Extended Fuse |    0xFD |
avrdude arguments: `-U lfuse:w:0xff:m -U hfuse:w:0xD8:m -U efuse:w:0xfd:m`

**debugging:**

| Fuse          | Setting |
|:--------------|:--------|
| Low Fuse      |    0xFF |
| High Fuse     |    0x18 |
| Extended Fuse |    0xFD |

(Both `OCDEN` and `JTAGEN` fuse bits are enabled)

avrdude arguments: `-U lfuse:w:0xff:m -U hfuse:w:0x18:m -U efuse:w:0xfd:m`

A useful tool to calculate fuse settings yourself is this [fuse
calculator](http://www.engbedded.com/fusecalc/), which also works with other AVR
MCUs.

### Debugging RIOT on the Arduino Mega 2560

`make BOARD=arduino-mega2560 debug-server`: starts an
[avarice](http://avarice.sourceforge.net/) (avarice needs to be installed)
server that `avr-gdb` can connect to.

`make BOARD=arduino-mega2560 debug`: starts an avarice server and connects
`avr-gdb` to it.

**Note:** To flash the board via the ISP while debugging the additional flag
`PROGRAMMER=dragon_isp` is required.
For a full rebuild and debug cycle use the following command:

`make BOARD=arduino-mega2560 PROGRAMMER=dragon_isp clean all flash debug`

@note       If you are using a different debugger than the Atmel-ICE, you have
            to export the `AVR_DEBUGDEVICE` environment variable to the required
            flag to pass to AVaRICE, e.g. when using the AVR Dragon you have to
            export `AVR_DEBUGDEVICE=--dragon`. If the debug device is not
            connected via USB, you also need to export `AVR_DEBUGINTERFACE` to
            the correct value.

# Mac OSX El Capitan users
Mac users can flash this Arduino board by installing `avr-gcc` and `avrdude`
from `brew`.
Debug is possible but is not covered in this wiki, if you need it, please
refer to this [page](https://www.obdev.at/products/crosspack/index.html).

## Toolchain installation
You should have installed `brew` to follow this instructions. Otherwise,
[install](http://digitizor.com/install-homebrew-osx-el-capitan/) it.

Then, add a repository:

```
$ brew tap osx-cross/avr
```

Afterwards, install `avr-gcc`:

```
$ brew install avr-libc
```

And finally:

```
$ brew install avrdude --with-usb
```

With this you should be allowed to compile and to flash code to the Arduino
Mega.

## Troubleshooting for serial connection
In OSX El Capitan, there is no native driver working for the serial
connection.

In order to install it, you must download and install a CDC-ACM driver from
[here](https://www.aten.com/global/en/products/usb-&-thunderbolt/usb-converters/uc232a/)
(Go to Support and Downloads/Software & Driver/Mac Software).

A reboot should be enough to find your Arduino on `/dev/tty.usbmodem*`
