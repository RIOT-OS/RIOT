@defgroup    boards_atmega328p_xplained_mini ATmega328p Xplained Mini
@ingroup     boards
@brief       Support for using the ATmega328p Xplained Mini board

## Overview

The ATmega328p MCU is most popular in the Arduino UNO.  The Xplained Mini board
is a reduced version of Arduino UNO with a EDBG CMSIS-DAP programmer.

The Xplained Mini has two internal oscillators, one clocked at 8MHz and one at
128kHz and one external 16/8MHz clock at 5/3.3V.  The default fuses of the
Xplained Mini are configured to use external 16MHz oscillator.  This default
configuration uses a 5.0V supply voltage.

### MCU
| MCU           | ATmega328p                                    |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATmega                                    |
| Vendor        | Microchip (previously Atmel)                  |
| RAM           | 2KiB                                          |
| Flash         | 32KiB                                         |
| EEPROM        | 1KiB                                          |
| Frequency     | 1MHz/8MHz/16MHz                               |
| Timers        | 3 (2x 8bit, 1x 16bit)                         |
| ADCs          | 6 analog input pins                           |
| UARTs         | 1                                             |
| SPIs          | 1                                             |
| I2Cs          | 1 (called TWI)                                |
| Vcc           | 5.0V USB (when clocked at 16MHz)              |
| Datasheet     | [Official datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf) |
| User Manual   | [User Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328P-Xplained-Mini-UG-DS50002659B.pdf) |
| Design files  | [Design files](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328P-Xplained-Mini_Design-Documentation.zip) |
| AN 42552      | [AN_42552](http://ww1.microchip.com/downloads/en/Appnotes/Atmel-42552-AVR-for-IoT-Using-megaAVR-ATWINC1500-802-11bgn-WiFi-Network-Controller-Module_ApplicationNote_AT13041.pdf) |

### Clock Frequency

The Xplained Mini has two internal oscillators clocked at 8MHz and at 128kHz
that allow it to be operated without any external clock source or crystal.  By
default the fuses are configured to use the external 16MHz clock without clock
divider resulting in a CPU clock speed of 16MHz.  There is no need to clear
the `CKDIV8` fuse once it is already done.

    default fuses: (E:FC, H:D1, L:E0)

This "board" is configured to use 16MHz as core clock, so that the ATmega328p
runs at the highest frequency possible using the external clock source.

By setting the environment variable `ATMEGA328P_CLOCK` to a custom frequency
in Hz (e.g. `1000000` for 1MHz), this core clock can be changed easily. Refer
to the datasheet on how to configure the ATmega328p to use an external crystal,
an external clock source or the clock divider.

### Relation Between Supply Voltage, Clock Frequency and Power Consumption

A higher supply voltage results in a higher current drawn. Thus, lower power
consumption can be achieved by using a lower supply voltage. However, higher
clock frequencies require higher supply voltages for reliable operation.

The lowest possible supply voltage at 8 MHz is 2.7V (with some safety margin),
which results in an active supply current of less than 3 mA (about 8 mW power
consumption) according to the datasheet. At 1 MHz core clock a supply voltage
of 1.8V is possible resulting in an active supply current of less than 0.3 mA
(about 0.5 mW power consumption). For more details, refer to the official
datasheet.

## Flashing the Device

The only requirement is `avrdude` tool installed:

    make BOARD=atmega328P-xplained-mini flash

## Serial Terminal

The AVR Xplained Mini already have an USB CDC converter build in.  So, simple
type to get terminal:

    make BOARD=atmega328P-xplained-mini term

## On-Chip Debugging (OCD)

E.g. with the AVR Xplained Mini and
[AVaRICE](https://github.com/avrdudes/avarice)
you can debug the ATmega328P using the debugWIRE interface.  Compared to the
ATmega MCUs with JTAG interface the debug facilities are however significantly
reduced: Only a single hardware breakpoint and no watchpoints are supported.
The hardware breakpoint is used for single-stepping.  If you set breakpoints,
the AVR Xplained Mini will transparently replace the instruction to break upon
with a break instruction.  Once the breakpoint is hit, the break instruction
is overwritten with the original instruction.  Thus, every breakpoint hit
cause two flash cycles to be performed, which not only results in a slow
debugging experience, but also causes significant wear.

In order to enable debugWIRE run, replace `<PROGRAMMER>` with the
`xplainedmini_dw`:

    avrdude -c <PROGRAMMER> -p m328p -U hfuse:w:0x99:m

You can disable it again via:

    avrdude -c <PROGRAMMER> -p m328p -U hfuse:w:0xd9:m

@warning    As the reset pin is repurposed for debugWIRE, a regular ISP will
            not be able to disable the debugWIRE interface anymore.  The AVR
            Xplained Mini can temporary disable the debugWIRE interface via a
            debugWIRE command and avrdude will do so when debugWIRE is used.

For debugging, the ATmega328P Xplained Mini already have a debugger.  Once the
debugWIRE is enabled via the fuse settings, you can start debugging via:

    make debug

If this fails after flashing the ATmega328P, it is like due to debugWIRE being
temporary disabled by avrdude in order to use the ISP feature.  Perform a
cold boot and the ATmega328P will have debugWIRE enabled again.

@note       If you are using a different debugger than the AVR Xplained Mini,
            you have to export the `AVR_DEBUGDEVICE` environment variable to
	    the required flag to pass to AVaRICE, e.g. when using the Atmel-ICE
	    you have to export `AVR_DEBUGDEVICE=--edbg`. If the debug device
	    is not connected via USB, you also need to export
	    `AVR_DEBUGINTERFACE` to the correct value.

There is a how to ATWINC1500 802.11 b/g/n Wi-Fi Network with this board
(AN_42552).
