@defgroup   boards_sltb009a Silicon Labs SLTB009A starter kit
@ingroup    boards
@brief      Support for Silicon Labs SLTB009A starter kit

## Overview

Silicon Labs Thunderboard GG12 is equipped with the EFM32 microcontroller.
It is specifically designed for low-power applications, having energy-saving
peripherals, different energy modes and short wake-up times.

The starter kit is equipped with an Advanced Energy Monitor. This allows you to
actively measure the power consumption of your hardware and code, in real-time.

## Hardware

### MCU

| MCU             | EFM32GG12B810F1024GM64                               |
|-----------------|------------------------------------------------------|
| Family          | ARM Cortex-M4F                                       |
| Vendor          | Silicon Labs                                         |
| Vendor Family   | EFM32 Giant Gecko 12B                                |
| RAM             | 192.0 KiB                                            |
| Flash           | 1024.0 KiB                                           |
| EEPROM          | no                                                   |
| Frequency       | up to 50 MHz                                         |
| FPU             | yes                                                  |
| MPU             | yes                                                  |
| DMA             | 12 channels                                          |
| Timers          | 4 x 32-bit, 7 x 16-bit + 1 x 16-bit (low power)      |
| ADCs            | 12-bit ADC                                           |
| DACs            | 2 x 12-bit VDAC (500 ksamples/s), 1 x IDAC           |
| I2Cs            | 2 x                                                  |
| SPIs            | 5 x USART                                            |
| UARTs           | 2 x UART, 5 x USART, 1 x LEUART                      |
| USB             | 1 x Low Energy Full-Speed USB 2.0                    |
| Vcc             | 1.8 V - 3.8 V                                        |
| Datasheet       | [Datasheet](https://www.silabs.com/documents/public/data-sheets/efm32gg12-datasheet.pdf) |
| Manual          | [Manual](https://www.silabs.com/documents/public/reference-manuals/efm32gg12-rm.pdf) |
| Board Manual    | [Board Manual](https://www.silabs.com/documents/public/user-guides/ug371-sltb009a-user-guide.pdf) |
| Board Schematic | Can be downloaded using Silicon Labs' Simplicity Studio |

### Pinout

This is the pinout of the expansion header of the board.
PIN 1 is the top-left contact.

| RIOT Peripheral | Name | PIN | PIN | Name | RIOT Peripheral |
|-----------------|------|-----|-----|------|-----------------|
|                 | GND  | 1   | 2   | VMCU |                 |
|                 | PD0  | 3   | 4   | PA0  | SPI_DEV(0):MOSI |
|                 | PD1  | 5   | 6   | PA1  | SPI_DEV(0):MISO |
|                 | PA4  | 7   | 8   | PA2  | SPI_DEV(0):CLK  |
|                 | PA5  | 9   | 10  | PA3  |                 |
|                 | PA6  | 11  | 12  | PC4  | UART_DEV(1):TX  |
|                 | PE15 | 13  | 14  | PC5  | UART_DEV(1):RX  |
| I2C_DEV(0):SCL  | PE5  | 15  | 16  | PE4  | I2C_DEV(0):SCL  |
|                 |      | 17  | 18  | 5V   |                 |
|                 |      | 19  | 20  | 3V3  |                 |
|                 | PB12 | 21  | 22  | PE8  |                 |
| DAC_LINE(0)     | PB11 | 23  | 24  | PE9  |                 |
|                 | PB3  | 25  | 26  | PE10 |                 |
|                 | PB4  | 27  | 28  | PE11 |                 |
|                 | PD2  | 29  | 30  | PE13 |                 |
|                 | PD3  | 31  | 32  | PE14 |                 |
|                 | PD4  | 33  | 34  | PF5  |                 |

### Peripheral mapping

| Peripheral  | Number  | Hardware   | Pins             | Comments                            |
|-------------|---------|------------|------------------|-------------------------------------|
| ADC         | 0       | ADC0:CH0   |                  | Internal temperature                |
| ADC         | 1       | ADC0:CH1   |                  | AVDD                                |
| I2C         | 0       | I2C0       | SDA:PE4, SCL:PE5 | Normal speed                        |
| HWCRYPTO    | -       | -          |                  | AES128/AES256, SHA1, SHA224/SHA256  |
| HWRNG       | -       | TNRG0      |                  | True Random Number Generator (TRNG) |
| RTT         | -       | RTCC       |                  | 1 Hz interval, either RTT or RTC    |
| RTC         | -       | RTCC       |                  | 1 Hz interval, either RTT or RTC    |
| SPI         | 0       | USART3     | MOSI:PA0, MISO:PA1, CLK:PA2 |                          |
| Timer       | 0       | TIMER0 + TIMER1 |             | TIMER0 is used as prescaler         |
| Timer       | 1       | LETIMER0   |                  |                                     |
| UART        | 0       | USART0     | RX:PE6, TX:PE7   | Default STDIO                       |
| UART        | 1       | UART0      | RX:PC5, TX:PC4   |                                     |

### User interface

| Peripheral | Mapped to | Pin       | Comments   |
|------------|-----------|-----------|------------|
| Button     | PB0_PIN   | PD5       |            |
|            | PB1_PIN   | PD8       |            |
| LED        | LED0R_PIN | PA12      |            |
|            | LED0G_PIN | PA13      |            |
|            | LED0B_PIN | PA14      |            |
|            | LED1R_PIN | PD6       |            |
|            | LED1G_PIN | PF12      |            |
|            | LED1B_PIN | PE12      |            |
|            | LED0_PIN  | LED0R_PIN |            |
|            | LED1_PIN  | LED1R_PIN |            |

## Implementation Status

| Device           | ID         | Supported | Comments                                           |
|------------------|------------|-----------|----------------------------------------------------|
| MCU              | EFM32GG12B | yes       | Power modes supported                              |
| Low-level driver | ADC        | yes       |                                                    |
|                  | DAC        | yes       | VDAC, IDAC is not supported                        |
|                  | Flash      | yes       |                                                    |
|                  | GPIO       | yes       | Interrupts are shared across pins (see ref manual) |
|                  | HW Crypto  | yes       |                                                    |
|                  | I2C        | yes       |                                                    |
|                  | PWM        | yes       |                                                    |
|                  | RTCC       | yes       | As RTT or RTC                                      |
|                  | SPI        | yes       | Only master mode                                   |
|                  | Timer      | yes       |                                                    |
|                  | UART       | yes       | USART is shared with SPI. LEUART baud rate limited |
|                  | USB        | yes       | Device mode                                        |

## Board configuration

### Board controller

The starter kit is equipped with a Board Controller. This controller provides
a virtual serial port.

**Note:** the board controller *always* configures the virtual serial port at
115200 baud with 8 bits, no parity and one stop bit. This also means that it
expects data from the MCU with the same settings.

### Clock selection

There are several clock sources that are available for the different
peripherals. You are advised to read
[AN0004.0](https://www.silabs.com/documents/public/application-notes/an0004.0-efm32-cmu.pdf)
to get familiar with the different clocks.

| Source | Internal | Speed      | Comments                           |
|--------|----------|------------|------------------------------------|
| HFRCO  | Yes      | 19 MHz     | Enabled during startup, changeable |
| HFXO   | No       | 50 MHz     |                                    |
| LFRCO  | Yes      | 32.768 kHz |                                    |
| LFXO   | No       | 32.768 kHz |                                    |
| ULFRCO | No       | 1 kHz      | Not very reliable as a time source |

The sources can be used to clock following branches:

| Branch | Sources                 | Comments                     |
|--------|-------------------------|------------------------------|
| HF     | HFRCO, HFXO             | Core, peripherals            |
| LFA    | LFRCO, LFXO             | Low-power timers             |
| LFB    | LFRCO, LFXO, CORELEDIV2 | Low-power UART               |
| LFE    | LFRCO, LFXO             | Real-time Clock and Calendar |

CORELEDIV2 is a source that depends on the clock source that powers the core.
It is divided by 2 or 4 to not exceed maximum clock frequencies (EMLIB takes
care of this).

The frequencies mentioned in the tables above are specific for this starter
kit.

It is important that the clock speeds are known to the code, for proper
calculations of speeds and baud rates. If the HFXO or LFXO are different from
the speeds above, ensure to pass `EFM32_HFXO_FREQ=freq_in_hz` and
`EFM32_LFXO_FREQ=freq_in_hz` to your compiler.

You can override the branch's clock source by adding `CLOCK_LFA=source` to your
compiler defines, e.g. `CLOCK_LFA=cmuSelect_LFRCO`.

### Low-power peripherals

The low-power UART is capable of providing an UART peripheral using a low-speed
clock. When the LFB clock source is the LFRCO or LFXO, it can still be used in
EM2. However, this limits the baud rate to 9600 baud. If a higher baud rate is
desired, set the clock source to CORELEDIV2.

**Note:** peripheral mappings in your board definitions will not be affected by
this setting. Ensure you do not refer to any low-power peripherals.

### RTC or RTT

RIOT-OS has support for *Real-Time Tickers* and *Real-Time Clocks*.

However, this board MCU family has support for a 32-bit *Real-Time Clock and
Calendar*, which can be configured in ticker mode **or** calendar mode.
Therefore, only one of both peripherals can be enabled at the same time.

Configured at 1 Hz interval, the RTCC will overflow each 136 years.

### Hardware crypto

This MCU is equipped with a hardware-accelerated crypto peripheral that can
speed up AES128, AES256, SHA1, SHA256 and several other cryptographic
computations.

A peripheral driver interface is proposed, but not yet implemented.

### Usage of EMLIB

This port makes uses of EMLIB by Silicon Labs to abstract peripheral registers.
While some overhead is to be expected, it ensures proper setup of devices,
provides chip errata and simplifies development. The exact overhead depends on
the application and peripheral usage, but the largest overhead is expected
during peripheral setup. A lot of read/write/get/set methods are implemented as
inline methods or macros (which have no overhead).

Another advantage of EMLIB are the included assertions. These assertions ensure
that peripherals are used properly. To enable this, pass `DEBUG_EFM` to your
compiler.

### Pin locations

The EFM32 platform supports peripherals to be mapped to different pins
(predefined locations). The definitions in `periph_conf.h` mostly consist of a
location number and the actual pins. The actual pins are required to configure
the pins via GPIO driver, while the location is used to map the peripheral to
these pins.

In other words, these definitions must match. Refer to the data sheet for more
information.

This MCU has extended pin mapping support. Each pin of a peripheral can be
connected separately to one of the predefined pins for that peripheral.

## Flashing the device

The board provides a on-board J-Link debugger through the micro USB board so
that flashing and debugging is very easy.

Flashing is supported by RIOT-OS using the command below:
```
make flash
```

To run the GDB debugger, use the command:
```
make debug
```

Or, to connect with your own debugger:
```
make debug-server
```

Some boards have (limited) support for emulation, which can be started with:
```
make emulate
```

## Supported Toolchains

For using the Silicon Labs SLTB009A starter kit we strongly recommend
the usage of the
[GNU Tools for ARM Embedded Processors](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
toolchain.

## License information

Silicon Labs' EMLIB: zlib-style license (permits distribution of source).
