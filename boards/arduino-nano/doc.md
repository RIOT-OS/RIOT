@defgroup    boards_arduino-nano Arduino Nano
@ingroup     boards
@brief       Support for the Arduino Nano board

## Overview

The Arduino Nano is the cheapest member of the Arduino family. It is based on
Atmel's AVR architecture and sports an ATmega328p MCU. It is like many Arduinos
extensible by using shields.

### MCU
| MCU           | ATmega328p                                    |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATmega                                    |
| Vendor        | Atmel                                         |
| RAM           | 2 KiB                                         |
| Flash         | 32 KiB (2 KiB reserved for the bootloader)    |
| Frequency     | 16 MHz                                        |
| Timers        | 3 (2x 8bit, 1x 16bit)                         |
| ADCs          | 6 analog input pins                           |
| UARTs         | 1                                             |
| SPIs          | 1                                             |
| I2Cs          | 1 (called TWI)                                |
| Vcc           | 5.0V                                          |
| MCU Datasheet | [ATmega328p datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf) |
| Board Manual  | [Board Manual](https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf) |

## Flashing the Device
Flashing RIOT on the Arduino Nano is quite straight forward, just connect your
Arduino Nano via the USB connector to your host computer and type:

`make BOARD=arduino-nano flash`

This should take care of everything!

We use the open `avrdude` tool to write the new code into the ATmega328p's
flash

## Using Optiboot

You can use the [Optiboot](https://github.com/Optiboot/optiboot) bootloader
instead of the stock bootloader for faster programming. Optiboot also is
smaller (512 byte instead of 2 KiB), so that 1.5 KiB more program memory is
available for RIOT. Refer to the project page for instructions on how to
build an flash the bootloader. Don't forgot to also update the fuse settings
to set the bootloader size to 256 words (512 bytes).

Compile and flash with `make BOARD=arduino-nano ATMEGA_BOOTLOADER=optiboot flash` or
use `export ARDUINO_NANO_BOOTLOADER=optiboot` in order to not have to specify
the bootloader during compilation and flashing.

## Issues

### RIOT Stuck in Reboot Loop

If RIOT is stuck in a reboot loop e.g. after restarting the device with the
`reboot` shell command, this is likely caused by an
[issue with the stock bootloader](https://forum.arduino.cc/index.php?topic=150419.0)
that can be solved by using Optiboot as bootloader instead (see above).

## On-Chip Debugging
On-Chip Debugging on the Arduino Nano is not supported via the usual JTAG
interface used in ATmega MCUs with higher pin counts, but via debugWIRE. While
debugWIRE has the advantage of only using the RESET pin to transfer data, the
features provided are extremely limited. If the same issue can be reproduced on
an Arduino Mega2560, which supports JTAG, it will be much easier and more
productive to debug your code on the Arduino Mega2560. If the bug cannot be
reproduced, limited on chip debugging is possible on the Arduino Nano
nonetheless.

### Prerequisites
#### Debugging Hardware
In order to be able to use On-Chip Debugging you will need the AVR Dragon, which
is the ~~cheapest~~ least expensive programmer and debugger available that
supports programming via SPI ("normal ISP"), High Voltage Serial Programming,
and Parallel Programming, as well as debugging via JTAG, debugWIRE, PDI and
aWire. So at least can use it for just about every AVR device.

#### Board Modifications
On the Arduino Nano the RESET pin of the MCU is connected to a 100 nF capacitor,
which in turn is connected to the DTR pin of the FT232RL USB-UART bridge. This
allows the device to be automatically reset when you connected to the board
via a serial. This is particularly useful during programming via the bootloader
(without external ISP programmer), as avrdude can trigger the reset and, thus,
start the bootloader without the user having to press a button.

In order to use on-chip debugging, the capacitor needs however to be
disconnected from the reset pin. You can either carefully de-solder it (which
allows you to solder it back in after debugging), or just break it off with
pinch-nose pliers (which usually destroys the capacitor, making the modification
permanent). After this modification, flashing via bootloader requires a manual
press on the reset button.

#### Software
You need to have [AVaRICE](https://github.com/avrdudes/avarice) installed. Some
distros have this packaged already.

#### Fuses
In order to use On-Chip Debugging, the `DWEN` bit in the high fuse needs to be
enabled (set to zero). The exact fuse settings for debugging and the default
fuse setting are these:

| Fuse          | Default Setting | Debug Setting |
|:------------- |:--------------- |:------------- |
| Low Fuse      | `0xFF`          | `0xFF`        |
| High Fuse     | `0xDA`          | `0x9A`        |
| Extended Fuse | `0xFD`          | `0xFD`        |

You can enable debugWIRE debugging by running (replace `<PROGRAMMER>` by the
name of your programmer, e.g. `dragon_isp` in case of the AVR Dragon):

    avrdude -p m328p -c <PROGRAMMER> -U hfuse:w:0x9a:m

And disable debugging via:

    avrdude -p m328p -c <PROGRAMMER> -U hfuse:w:0xda:m


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

    make BOARD=arduino-nano debug

@warning    For flashing the device via ISP, avrdude will temporarily disable
            debugWIRE. If AVaRICE complains that synchronization with the device
            is not possible after having it flashed, the device might need a
            cold boot to enable debugWIRE again.

The memory map of the ELF file does not take the bootloader into account. The
author of this text used an ISP to program the Arduino Nano during debugging to
avoid any issues. You might want to do the same, e.g. via:

    make BOARD=arduino-nano PROGRAMMER=dragon_isp flash

@warning    Flashing via ISP overwrites the bootloader. But you can restore it
            easily using the ISP. Consult the Arduino documentation on how to
            restore the bootloader.

@note       If you are using a different debugger than the AVR Dragon, you have
            to export the `AVR_DEBUGDEVICE` environment variable to the required
            flag to pass to AVaRICE, e.g. when using the Atmel-ICE you have to
            export `AVR_DEBUGDEVICE=--edbg`. If the debug device is not
            connected via USB, you also need to export `AVR_DEBUGINTERFACE` to
            the correct value.

## Caution
Don't expect having a working network stack due to very limited resources.
