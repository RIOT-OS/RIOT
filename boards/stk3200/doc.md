@defgroup   boards_stk3200 Silicon Labs STK3200 starter kit
@ingroup    boards
@brief      Support for Silicon Labs STK3200 starter kit

## Overview
Silicon Labs EFM32 Zero Gecko Starter Kit is equipped with the EFM32
microcontroller. It is specifically designed for low-power applications, having
energy-saving peripherals, different energy modes and short wake-up times.

The starter kit is equipped with an Advanced Energy Monitor. This allows you to
actively measure the power consumption of your hardware and code, in real-time.

## Hardware

### MCU
| MCU             | EFM32ZG222F32                                                                              |
|-----------------|--------------------------------------------------------------------------------------------|
| Family          | ARM Cortex-M0PLUS                                                                          |
| Vendor          | Silicon Labs                                                                               |
| Vendor Family   | EFM32 Zero Gecko                                                                           |
| RAM             | 4.0 KiB                                                                                    |
| Flash           | 32.0 KiB                                                                                   |
| EEPROM          | no                                                                                         |
| Frequency       | up to 24 MHz                                                                               |
| FPU             | no                                                                                         |
| MPU             | no                                                                                         |
| DMA             | 12 channels                                                                                |
| Timers          | 2x 16-bit                                                                                  |
| ADCs            | 12-bit ADC                                                                                 |
| UARTs           | 2x USART, 1x LEUART                                                                        |
| SPIs            | 2x USART                                                                                   |
| I2Cs            | 1x                                                                                         |
| Vcc             | 1.98 V - 3.8 V                                                                             |
| Datasheet       | [Datasheet](https://www.silabs.com/documents/public/data-sheets/efm32zg-datasheet.pdf)     |
| Manual          | [Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32ZG-RM.pdf)         |
| Board Manual    | [Board Manual](https://www.silabs.com/documents/public/user-guides/efm32zg-stk3200-ug.pdf) |
| Board Schematic | Can be downloaded using Silicon Labs' Simplicity Studio                                    |

### Pinout
This is the pinout of the expansion header on the right side of the board.
PIN 1 is the bottom-left contact when the header faces  you horizontally.

|      | PIN | PIN |      |
|------|-----|-----|------|
| 3V3  | 20  | 19  | RES  |
| 5V   | 18  | 17  | RES  |
| PE12 | 16  | 15  | PE13 |
| PD5  | 14  | 13  | PA1  |
| PD4  | 12  | 11  | PB11 |
| PC14 | 10  | 9   | PA0  |
| PC15 | 8   | 7   | PC2  |
| PD6  | 6   | 5   | PC1  |
| PD7  | 4   | 3   | PC0  |
| VMCU | 2   | 1   | GND  |

**Note**: not all starter kits by Silicon Labs share the same pinout!

**Note:** some pins are connected to the board controller, when enabled!

### Peripheral mapping
| Peripheral | Number  | Hardware        | Pins                            | Comments                                                 |
|------------|---------|-----------------|---------------------------------|----------------------------------------------------------|
| ADC        | 0       | ADC0            | CHAN0: internal temperature     | Ports are fixed, 14/16-bit resolution not supported      |
| I2C        | 0       | I2C0            | SDA: PE12, SCL: PE13            | `I2C_SPEED_LOW` and `I2C_SPEED_HIGH` clock speed deviate |
| HWCRYPTO   | &mdash; | &mdash;         |                                 | AES128/AES256 only                                       |
| RTT        | &mdash; | RTC             |                                 | Either RTT or RTC (see below)                            |
| RTC        | &mdash; | RTC             |                                 | Either RTC or RTT (see below)                            |
| SPI        | 0       | USART1          | MOSI: PD7, MISO: PD6, CLK: PC15 |                                                          |
| Timer      | 0       | TIMER0 + TIMER1 |                                 | TIMER0 is used as prescaler (must be adjacent)           |
| UART       | 0       | LEUART0         | RX: PD5, TX: PD4                | STDIO Output, Baud rate limited (see below)              |

### User interface
| Peripheral | Mapped to | Pin  | Comments   |
|------------|-----------|------|------------|
| Button     | PB0_PIN   | PC8  |            |
|            | PB1_PIN   | PC9  |            |
| LED        | LED0_PIN  | PC10 | Yellow LED |
|            | LED1_PIN  | PC11 | Yellow LED |

## Implementation Status
| Device           | ID          | Supported | Comments                                                       |
|------------------|-------------|-----------|----------------------------------------------------------------|
| MCU              | EFM32ZG     | yes       | Power modes supported                                          |
| Low-level driver | ADC         | yes       |                                                                |
|                  | Flash       | yes       |                                                                |
|                  | GPIO        | yes       | Interrupts are shared across pins (see reference manual)       |
|                  | HW Crypto   | yes       |                                                                |
|                  | I2C         | yes       |                                                                |
|                  | PWM         | yes       |                                                                |
|                  | RTC         | yes       | As RTT or RTC                                                  |
|                  | SPI         | partially | Only master mode                                               |
|                  | Timer       | yes       |                                                                |
|                  | UART        | yes       | USART is shared with SPI. LEUART baud rate limited (see below) |
|                  | USB         | no        |                                                                |
| LCD driver       | LS013B7DH03 | yes       | Sharp Low Power Memory LCD via the U8g2 package                |

## Board configuration

### Board controller
The starter kit is equipped with a Board Controller. This controller provides a
virtual serial port. The board controller is enabled via a GPIO pin.

By default, this pin is enabled. You can disable the board controller module by
passing `DISABLE_MODULE=silabs_bc` to the `make` command.

**Note:** to use the virtual serial port, ensure you have the latest board
controller firmware installed.

**Note:** the board controller *always* configures the virtual serial port at
115200 baud with 8 bits, no parity and one stop bit. This also means that it
expects data from the MCU with the same settings.

The low power LCD is also used by the board controller when the `DISP_SELECTED`
pin is low. This pin is not initialized by the board, so you have to ensure
this pin is initialized by your application if you want to control the low
power LCD.

### Clock selection
There are several clock sources that are available for the different
peripherals. You are advised to read [AN0004.0](https://www.silabs.com/documents/public/application-notes/an0004.0-efm32-cmu.pdf)
to get familiar with the different clocks.

| Source | Internal | Speed      | Comments                           |
|--------|----------|------------|------------------------------------|
| HFRCO  | Yes      | 14 MHz     | Enabled during startup, changeable |
| HFXO   | No       | 24 MHz     |                                    |
| LFRCO  | Yes      | 32.768 kHz |                                    |
| LFXO   | No       | 32.768 kHz |                                    |
| ULFRCO | No       | 1 kHz      | Not very reliable as a time source |

The sources can be used to clock following branches:

| Branch | Sources                 | Comments                     |
|--------|-------------------------|------------------------------|
| HF     | HFRCO, HFXO             | Core, peripherals            |
| LFA    | LFRCO, LFXO             | Low-power timers             |
| LFB    | LFRCO, LFXO, CORELEDIV2 | Low-power UART               |

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

However, this board MCU family has support for a 24-bit *Real-Time Counter*
only, which is a ticker only. A compatibility layer for ticker-to-calendar is
available, but this includes extra code size to convert from timestamps to time
structures and visa versa.

Configured at 1 Hz interval, the RTC will overflow each 194 days. When using
the ticker-to-calendar mode, this interval is extended artificially.

### Hardware crypto
This MCUs has support for hardware-accelerated AES128.

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

## Flashing the device
To flash, [SEGGER JLink](https://www.segger.com/jlink-software.html) is
required.

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
For using the Silicon Labs STK3200 starter kit we strongly recommend
the usage of the [GNU Tools for ARM Embedded Processors](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
toolchain.

## License information
Silicon Labs' EMLIB: zlib-style license (permits distribution of source).
