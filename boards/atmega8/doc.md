@defgroup    boards_atmega8 Standalone ATmega8
@ingroup     boards
@brief       Support for using the ATmega8 as standalone board

## Overview

As the ATmega8 can run from the internal oscillator, placing it on a breadboard,
connecting an USB-UART adapter and power is enough to run RIOT on it. (An ISP
programmer will be needed to program it; or to program a bootloader to
subsequently allow programming via UART.)

### MCU
| MCU           | ATmega8                                       |
|:------------- |:--------------------------------------------- |
| Family        | AVR/ATmega                                    |
| Vendor        | Microchip (previously Atmel)                  |
| RAM           | 1KiB                                          |
| Flash         | 8KiB                                          |
| EEPROM        | 512B                                          |
| Frequency     | 1MHz/8MHz (up to 16MHz with external clock)   |
| Timers        | 3 (2x 8bit, 1x 16bit)                         |
| ADCs          | 6 analog input pins                           |
| UARTs         | 1                                             |
| SPIs          | 1                                             |
| I2Cs          | 1 (called TWI)                                |
| Vcc           | 4.5V - 5.5V (ATmega8), 2.7V - 5.5V (ATmega8L) |
| Datasheet     | [Official datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2486-8-bit-AVR-microcontroller-ATmega8_L_datasheet.pdf) |

### Pinout

\htmlonly<style>div.image img[src="https://camo.githubusercontent.com/c55beef2f138da61fe671a1e4a307ff4ffbc318d/68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067"]{width:100%;}</style>\endhtmlonly
@image html "https://camo.githubusercontent.com/c55beef2f138da61fe671a1e4a307ff4ffbc318d/68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067" "Pinout of the ATmega328p"<br>

All credit for above pinout image goes to https://github.com/MCUdude/MiniCore#pinout

### Clock Frequency

The ATmega8 has an internal oscillators clocked at 1MHz that allow it to be
operated without any external clock source or crystal. By default the fuses are
configured to use the internal oscillator and an operating mode resulting in a
clock speed of 1MHz. By setting the `CKSEL` fuses to 0100 the clock will operate
at 8MHz without an external clock source. This can be done like this:

    avrdude -c usbasp -p m8 -B 32 -U lfuse:w:0xe4:m

(Replace `usbasp` with the ISP programmer you are using. The `-B 32` might
be needed on some ISP programmers to communicate with slow ATmega MCUs. It will
not be needed anymore after the clock device has been disabled.)

This "board" is configured to use 8MHz as core clock, so that the ATmega8
runs at the highest frequency possible without external clock sources.

By setting the environment variable `ATMEGA8_CLOCK` to a custom frequency in
Hz (e.g. `1000000` for 1MHz), this core clock can be changed easily. Refer to
the datasheet on how to configure the ATmega8 to use an external crystal, an
external clock source or the clock divider.

### Relation Between Supply Voltage, Clock Frequency and Power Consumption

A higher supply voltage results in a higher current drawn. Thus, lower power
consumption can be achieved by using a lower supply voltage. However, higher
clock frequencies require higher supply voltages for reliable operation.

The lowest possible supply voltage at 8 MHz is 2.7V (Atmega8L) or
4.5V (Atmega8).

## Flashing the Device

In order to flash the ATmega8 without a bootloader, an ISP programmer is
needed. Connect the programmer as follows:

| ISP pin  | ATmega8 pin    |
|:-------- |:-------------- |
| MISO     | 18/PB4/MISO    |
| VCC      | 7/VCC          |
| SCK      | 19/PB5/SCK     |
| MOSI     | 17/PB3/MOSI    |
| RESET    | 1/RESET        |
| Ground   | 22/GND         |

The tool `avrdude` needs to be installed. When using the `usbasp` running

    make BOARD=atmega8 flash

will take care of everything. To use the programmer `<FOOBAR>` instead, run

    make BOARD=atmega8 PROGRAMMER=<FOOBAR> flash

## Serial Terminal

Connect a TTL adapter with pins 2/RXD and 3/TXD an run

    make BOARD=atmega8 term

Please note that the supply voltage should be compatible with the logic level of
the TTL adapter. Usually everything between 3.3 V and 5 V should work.
