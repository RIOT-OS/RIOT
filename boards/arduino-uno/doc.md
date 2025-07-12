@defgroup    boards_arduino-uno Arduino Uno
@ingroup     boards
@brief       Support for the Arduino Uno board

## Overview

The Arduino Uno is one of the cheapest board to start and discover with
electronics and embedded coding. It is based on Atmel's AVR architecture and
sports an ATmega328p MCU. It is like many Arduinos extensible by using shields.

### MCU
| MCU                          | ATmega328p            |
|:---------------------------- |:--------------------- |
| Family                       | AVR/ATmega            |
| Vendor                       | Atmel/Microchip       |
| RAM                          | 2 KiB                 |
| Flash                        | 32 KiB                |
| Frequency                    | 16 MHz                |
| Timers                       | 3 (2x 8bit, 1x 16bit) |
| ADCs                         | 6 analog input pins   |
| UARTs                        | 1                     |
| SPIs                         | 1                     |
| I2Cs                         | 1 (called TWI)        |
| Vcc                          | 5.0 V                 |
| Datasheet / Reference Manual | [Datasheet and Reference Manual](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf) |
| Board Manual                 | [Board Manual](https://www.arduino.cc/en/Main/ArduinoBoardUno) |

## Flashing the device
Flashing RIOT on the Arduino Uno is quite straight forward, just connect your
Arduino Uno using the programming port to your host computer and type:

`make BOARD=arduino-uno flash`

This should take care of everything!

We use the open `avrdude` tool to write the new code into the ATmega328p's
flash

## Pin Change Interrupts

More pins can be used for hardware interrupts using the Pin Change
Interrupt feature. See @ref boards_common_atmega for details.

## Caution
Don't expect having a working network stack due to very limited resources.

## On-Chip Debugging
On-Chip Debugging on the Arduino Uno is not supported via the usual JTAG
interface used in ATmega MCUs with higher pin counts, but via debugWIRE. While
debugWIRE has the advantage of only using the RESET pin to transfer data, the
features provided are extremely limited. If the same issue can be reproduced on
an Arduino Mega2560, which supports JTAG, it will be much easier and more
productive to debug your code on the Arduino Mega2560. If the bug cannot be
reproduced, limited on chip debugging is possible on the Arduino Uno
nonetheless.

### Prerequisites
#### Debugging Hardware
In order to be able to use On-Chip Debugging you will need the AVR Dragon, which
is the ~~cheapest~~ least expensive programmer and debugger available that
supports programming via SPI ("normal ISP"), High Voltage Serial Programming,
and Parallel Programming, as well as debugging via JTAG, debugWIRE, PDI and
aWire. So at least can use it for just about every AVR device.

#### Board Modifications
On the Arduino Uno the RESET pin of the MCU is connected to a 100 nF capacitor,
which in turn is connected to the ATmega16U2 (or an CH340 TTL Adapter in case of
most clones). This allows the device to be automatically reset when you
connected to the board via a serial. This is particularly useful during
programming via the bootloader (without external ISP programmer), as avrdude
can trigger the reset and, thus, start the bootloader without the user having
to press a button.

In order to use on-chip debugging, the capacitor needs however to be
disconnected from the reset pin. With the original Arduino Uno this can be done
by cutting the solder jumper labeled "RESET EN". This can easily be resoldered
to restore the original state. Most clones do not have this solder jumper and
you will likely have to break off the usually surface mounted capacitor. A
multimeter can be used to detect which capacitor is connected to the reset pin.
Keep in mind that the capacitor will likely be destroyed when removed by force
and it will be difficult to restore the auto-reset feature of the clones.

#### Software
You need to have [AVaRICE](e539724b20) installed. Some
distros have this packaged already.

#### Fuses
In order to use On-Chip Debugging, the `DWEN` bit in the high fuse needs to be
enabled (set to zero). The exact fuse settings for debugging and the default
fuse setting are these:

| Fuse          | Default Setting | Debug Setting |
|:------------- |:--------------- |:------------- |
| Low Fuse      | `0xFF`          | `0xFF`        |
| High Fuse     | `0xDE`          | `0x9E`        |
| Extended Fuse | `0xFD`          | `0xFD`        |

You can enable debugWIRE debugging by running (replace `<PROGRAMMER>` by the
name of your programmer, e.g. `dragon_isp` in case of the AVR Dragon):

    avrdude -p m328p -c <PROGRAMMER> -U hfuse:w:0x9e:m

And disable debugging via:

    avrdude -p m328p -c <PROGRAMMER> -U hfuse:w:0xde:m


@note   You can use a different ISP to enable debugging, but disabling it
        again will only work with the AVR Dragon: The ISP will require the RESET
        pin to work, but the RESET pin is re-purposed for debugWIRE when
        debugging is enabled. Recent versions of avrdude will use the debugWIRE
        interface to temporarily disable debugWIRE and restore the RESET pin's
        default behavior in order to use the ISP. But this requires a
        programmer/debugger that can be used as both ISP and debugWIRE debugger
        using the same connector. So don't enable debugging unless you have an
        AVR Dragon or another plan on how to disable debugging again.

### Debugging
With the AVR Dragon, debugging is as simple as running:

    make BOARD=arduino-uno debug

@warning    For flashing the device via ISP, avrdude will temporarily disable
            debugWIRE. If AVaRICE complains that synchronization with the device
            is not possible after having it flashed, the device might need a
            cold boot to enable debugWIRE again.

The memory map of the ELF file does not take the bootloader into account. The
author of this text used an ISP to program the Arduino Uno during debugging to
avoid any issues. You might want to do the same, e.g. via:

    make BOARD=arduino-uno PROGRAMMER=dragon_isp flash

@warning    Flashing via ISP overwrites the bootloader. But you can restore it
            easily using the ISP. Consult the Arduino documentation on how to
            restore the bootloader.

@note       If you are using a different debugger than the AVR Dragon, you have
            to export the `AVR_DEBUGDEVICE` environment variable to the required
            flag to pass to AVaRICE, e.g. when using the Atmel-ICE you have to
            export `AVR_DEBUGDEVICE=--edbg`. If the debug device is not
            connected via USB, you also need to export `AVR_DEBUGINTERFACE` to
            the correct value.

### Breakpoints / Watchpoints
The ATmega328P only has a single hardware break point and zero watchpoints. The
single hardware breakpoint is used for single-stepping. As a result neither
breakpoints nor watchpoints can be used. AVaRICE tries to emulate breakpoints
be inserting the break machine instruction into the ROM in place of the
original instruction to break on. Once this break instruction is reached, the
original instruction is restored. This is not only super slow, but also
wastes two flash cycles every time a breakpoint is hit. This cumulates to
significant flash wear during long debugging sessions.
