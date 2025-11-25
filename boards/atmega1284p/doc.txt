/**
@defgroup    boards_atmega1284p Standalone ATmega1284P
@ingroup     boards
@brief       Support for using the ATmega1284P as standalone board

## Overview

As the ATmega1284P can run from the internal oscillator, placing it on a
breadboard, connecting an USB-UART adapter and power is enough to run RIOT on
it. (An ISP programmer will be needed to program it; or to program a bootloader
to subsequently allow programming via UART.)

### MCU
| MCU           | ATmega1284p                            |
|:------------- |:-------------------------------------- |
| Family        | AVR/ATmega                             |
| Vendor        | Microchip (previously Atmel)           |
| RAM           | 16KiB                                  |
| Flash         | 128KiB                                 |
| EEPROM        | 4KiB                                   |
| Frequency     | 8MHz (up to 20MHz with external clock) |
| Timers        | 3 (2x 8bit, 1x 16bit)                  |
| ADCs          | 6 analog input pins                    |
| UARTs         | 1                                      |
| SPIs          | 1                                      |
| I2Cs          | 1 (called TWI)                         |
| Vcc           | 2.7V - 5.5V (when clocked at 8MHz)     |
| Datasheet     | [Official datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42719-ATmega1284P_Datasheet.pdf) |

### Pinout

\htmlonly<style>div.image img[src="http://i.imgur.com/ayagBbM.png"]{width:50%;}</style>\endhtmlonly
@image html "http://i.imgur.com/ayagBbM.png" "Pinout of the ATmega1284P"<br>

The pinout image was posted in the
[Arduino Forum](https://forum.arduino.cc/index.php?topic=322745.0).
All credit goes to its poster, hansibull.

### Clock Frequency

The ATmega1284P has two internal oscillators clocked at 8MHz and at 128kHz that
allow it to be operated without any external clock source or crystal. By default
the fuses are configured to use the internal 8MHz oscillator, but the `CKDIV8`
fuse is set, so that the clock is divided down to 1MHz. By disabling the
`CKDIV8` fuse the clock will operate at 8MHz. This is highly encouraged, and
can be done with:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
avrdude -c atmelice -p m1284p -B 32 -U lfuse:w:0xc2:m
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(Replace `atmelice` with the programmer you are using. The -B 32 might be
needed for some programmers to successfully communicate with ATmegas clocked at
less than 2MHz. It will no longer be needed after disabling `CKDIV8`.)

By setting the environment variable `ATMEGA1284P_CLOCK` to a custom frequency in
Hz (e.g. `1000000` for 1MHz), this core clock can be changed easily. Refer to
the datasheet on how to configure the ATmega1284p to use an external crystal,
an external clock source or the clock divider.

### Relation Between Supply Voltage, Clock Frequency and Power Consumption

A higher supply voltage results in a higher current drawn. Thus, lower power
consumption can be achieved by using a lower supply voltage. However, higher
clock frequencies require higher supply voltages for reliable operation.

The lowest possible supply voltage at 8 MHz is 2.7V (with some safety margin).

## Flashing the Device

In order to flash the ATmega1284P without a bootloader, an ISP programmer is
needed. Connect the programmer as follows:

| ISCP pin | ATmega1284P    |
|:-------- |:-------------- |
| MISO     | 7/PB6/MISO     |
| VCC      | 10/VCC         |
| SCK      | 8/PB7/SCK      |
| MOSI     | 6/PB5/MOSI     |
| RESET    | 9/RESET        |
| Ground   | 11/GND         |

The tool `avrdude` needs to be installed. When using the Atmel ICE for
connected via JTAG for programming, running

    make BOARD=atmega1284p flash

will take care of everything. To use the programmer `<FOOBAR>` instead, run

    make BOARD=atmega1284p PROGRAMMER=<FOOBAR> flash

## Serial Terminal

Connect a TTL adapter with pins 14/RXD0 and 15/TXD0 an run

    make BOARD=atmega1284p term

Please note that the supply voltage should be compatible with the logic level of
the TTL adapter. Usually everything between 3.3 V and 5 V should work.

## On-Chip Debugging

In order to debug the ATmega1284P, an compatible debugger is needed. The Atmel
ICE is the ~~cheapest~~ least expensive option currently available. (But at
least it can program and debug pretty much all Atmel AVR and ARM chips.)

Once the Atmel ICE is correctly connected, the ATmega1284P has the JTAG
interface enabled, and the required software is installed, debugging can be
started using

    make debug

@note       If you are using a different debugger than the Atmel ICE, you have
            to export the `AVR_DEBUGDEVICE` environment variable to the required
            flag to pass to AVaRICE, e.g. when using the AVR Dragon you have to
            export `AVR_DEBUGDEVICE=--dragon`. If the debug device is not
            connected via USB, you also need to export `AVR_DEBUGINTERFACE` to
            the correct value.

#### Software Requirements

In order to debug you'll need an GDB version with AVR support and
[AVaRICE](http://avarice.sourceforge.net/). Note that AVaRICE sadly is not
being actively maintained and the latest release will not compile on most
systems. Thus, unless your distribution already ships a package of the SVN
version of AVaRICE, you'll have to build the tool from source.

### JTAG Pin Mapping

| Pin Name  | Pin   | Signal    | Atmel ICE Pin     |
|:----------|:------|:----------|:------------------|
| PC5       | 27    | TDI       | JTAG-9            |
| PC4       | 26    | TDO       | JTAG-3            |
| PC3       | 25    | TMS       | JTAG-5            |
| PC2       | 24    | TCK       | JTAG-1            |
| VCC       | 10    | VTG       | JTAG-4            |
| GND       | 11    | GND       | JTAG-2            |

### Fuse Settings

The `JTAGEN` fuse has to be set in order to use the JTAG interface. The JTAG
pins will no longer be available as GPIOs when this fuse is set. With the
default settings the MCUs are preprogrammed during manufacturing, the `JTAGEN`
fuse is already set. So with a new and unused package, you're ready directly
ready to go.

 */
