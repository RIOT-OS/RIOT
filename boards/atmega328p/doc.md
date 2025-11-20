@defgroup    boards_atmega328p Standalone ATmega328p
@ingroup     boards
@brief       Support for using the ATmega328p as standalone board

## Overview

The ATmega328p is most popular in the Arduino UNO. However, the 28 PDIP package
of the ATmega328p can easily be used without any "board": Just place it on a
bread board, and connect a TTL adapter and an ISP and you're ready to go.

The ATmega328p has two internal oscillators, one clocked at 8MHz and one at
128kHz. By default the fuses of the ATmega328p are configured that the internal
8MHz oscillator can be used. This allows the ATmega328p to be operated without
any external components at a supply voltage anywhere between 2.7V and 5.5V.

\htmlonly<style>div.image img[src="https://github.com/maribu/images/raw/master/ATmega328p.jpg"]{width:600px;}</style>\endhtmlonly
@image html "https://github.com/maribu/images/raw/master/ATmega328p.jpg" "ATmega328p DIP package on a breadboard"<br>

### MCU
| MCU           | ATmega328p                                    |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATmega                                    |
| Vendor        | Microchip (previously Atmel)                  |
| RAM           | 2KiB                                          |
| Flash         | 32KiB                                         |
| EEPROM        | 1KiB                                          |
| Frequency     | 1MHz/8MHz (up to 20MHz with external clock)   |
| Timers        | 3 (2x 8bit, 1x 16bit)                         |
| ADCs          | 6 analog input pins                           |
| UARTs         | 1                                             |
| SPIs          | 1                                             |
| I2Cs          | 1 (called TWI)                                |
| Vcc           | 2.7V - 5.5V (when clocked at 8MHz)            |
| Datasheet     | [Official datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf) |

### Pinout

\htmlonly<style>div.image img[src="https://camo.githubusercontent.com/c55beef2f138da61fe671a1e4a307ff4ffbc318d/68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067"]{width:100%;}</style>\endhtmlonly
@image html "https://camo.githubusercontent.com/c55beef2f138da61fe671a1e4a307ff4ffbc318d/68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067" "Pinout of the ATmega328p"<br>

All credit for above pinout image goes to https://github.com/MCUdude/MiniCore#pinout

### Clock Frequency

The ATmega328p has two internal oscillators clocked at 8MHz and at 128kHz that
allow it to be operated without any external clock source or crystal. By default
the fuses are configured to use the internal 8MHz oscillator and a clock divider
resulting in a clock speed of 1MHz. By clearing the `CKDIV8` fuse the clock
divider can be disabled to operate the ATmega328p at 8MHz without an external
clock source. This can be done like this:

    avrdude -c usbtiny -p m328p -B 32 -U lfuse:w:0xe2:m

(Replace `usbtiny` with the ISP programmer you are using. The `-B 32` might
be needed on some ISP programmers to communicate with slow ATmega MCUs. It will
not be needed anymore after the clock device has been disabled.)

This "board" is configured to use 8MHz as core clock, so that the ATmega328p
runs at the highest frequency possible without external clock sources.

By setting the environment variable `ATMEGA328P_CLOCK` to a custom frequency in
Hz (e.g. `1000000` for 1MHz), this core clock can be changed easily. Refer to
the datasheet on how to configure the ATmega328p to use an external crystal,
an external clock source or the clock divider.

### Relation Between Supply Voltage, Clock Frequency and Power Consumption

A higher supply voltage results in a higher current drawn. Thus, lower power
consumption can be achieved by using a lower supply voltage. However, higher
clock frequencies require higher supply voltages for reliable operation.

The lowest possible supply voltage at 8 MHz is 2.7V (with some safety margin),
which results in an active supply current of less than 3 mA (about 8 mW power
consumption) according to the datasheet. At 1 MHz core clock a supply voltage of
1.8V is possible resulting in an active supply current of less than 0.3 mA
(about 0.5 mW power consumption). For more details, refer to the official
datasheet.

## Flashing the Device

In order to flash the ATmega328P without a bootloader, an ICSP programmer is
needed. Connect the programmer as follows:

| ISCP pin | ATmega328p pin |
|:-------- |:-------------- |
| MISO     | 18/PB4/MISO    |
| VCC      | 7/VCC          |
| SCK      | 19/PB5/SCK     |
| MOSI     | 17/PB3/MOSI    |
| RESET    | 1/RESET        |
| Ground   | 22/GND         |

The tool `avrdude` needs to be installed. When using the `usbtiny` (or one of
the super cheap clones) running

    make BOARD=atmega328p flash

will take care of everything. To use the programmer `<FOOBAR>` instead, run

    make BOARD=atmega328p PROGRAMMER=<FOOBAR> flash

## Serial Terminal

Connect a TTL adapter with pins 2/RXD and 3/TXD an run

    make BOARD=atmega328p term

Please note that the supply voltage should be compatible with the logic level of
the TTL adapter. Usually everything between 3.3 V and 5 V should work.

## On-Chip Debugging (OCD)

E.g. with the AVR Dragon and [AVaRICE](https://github.com/avrdudes/avarice) you
can debug the ATmega328P using the debugWIRE interface. Compared to the ATmega
MCUs with JTAG interface the debug facilities are however significantly reduced:
Only a single hardware breakpoint and no watchpoints are supported. The
hardware breakpoint is used for single-stepping. If you set breakpoints, the
AVR Dragon will transparently replace the instruction to break upon with a
break instruction. Once the breakpoint is hit, the break instruction is
overwritten with the original instruction. Thus, every breakpoint hit cause two
flash cycles to be performed, which not only results in a slow debugging
experience, but also causes significant wear.

In order to enable debugWIRE run (replace `<PROGRAMMER>` with the programmer you
use, e.g. with `dragon_isp` if you use the AVR Dragon):

    avrdude -c <PROGRAMMER> -p m328p -U hfuse:w:0x99:m

You can disable it again via:

    avrdude -c <PROGRAMMER> -p m328p -U hfuse:w:0xd9:m

@warning    As the reset pin is repurposed for debugWIRE, a regular ISP will
            not be able to disable the debugWIRE interface anymore. The AVR
            Dragon can temporary disable the debugWIRE interface via a
            debugWIRE command and avrdude will do so when debugWIRE is used.
            So only enable debugWIRE if you have an AVR DRagon or other means
            to disable debugWIRE later on again.

For debugging, the ATmega328P needs to be connected to the AVR Dragon in the
very same way it needs to be connected for programming (see above). Once the
MCU is connected and debugWIRE is enabled via the fuse settings, you can start
debugging via:

    make debug

If this fails after flashing the ATmega328P, it is like due to debugWIRE
being temporary disabled by avrdude in order to use the ISP feature. Perform a
cold boot and the ATmega328P will have debugWIRE enabled again.

@note       If you are using a different debugger than the AVR Dragon, you have
            to export the `AVR_DEBUGDEVICE` environment variable to the required
            flag to pass to AVaRICE, e.g. when using the Atmel-ICE you have to
            export `AVR_DEBUGDEVICE=--edbg`. If the debug device is not
            connected via USB, you also need to export `AVR_DEBUGINTERFACE` to
            the correct value.

## Caution
Don't expect having a working network stack due to very limited resources ;-)
