@defgroup   boards_ikea-tradfri IKEA TRÅDFRI modules
@ingroup    boards
@brief      Support for the IKEA TRÅDFRI modules

## Overview
The IKEA TRÅDFRI is a small board found in different IKEA TRÅDFRI products.
The board contains an EFR32 microcontroller and a 2MBit SPI NOR Flash.

More information about the module can be found on this
[GitHub page](https://github.com/basilfx/TRADFRI-Hacking).

## Hardware

##
| MCU             | EFR32MG1P132F256GM32                                                                    |
|-----------------|-----------------------------------------------------------------------------------------|
| Family          | ARM Cortex-M4F                                                                          |
| Vendor          | Silicon Labs                                                                            |
| Vendor Family   | EFR32 Mighty Gecko 1P                                                                   |
| RAM             | 32.0 KiB (1.0 KiB reserved)                                                             |
| Flash           | 256.0 KiB                                                                               |
| EEPROM          | no                                                                                      |
| Frequency       | up to 38.4 MHz                                                                          |
| FPU             | yes                                                                                     |
| MPU             | yes                                                                                     |
| DMA             | 12 channels                                                                             |
| Timers          | 2x 16-bit + 1x 16-bit (low power)                                                       |
| ADCs            | 12-bit ADC                                                                              |
| UARTs           | 2x USART, 1x LEUART                                                                     |
| SPIs            | 2x USART                                                                                |
| I2Cs            | 1x                                                                                      |
| Vcc             | 1.85 V - 3.8 V                                                                          |
| Datasheet       | [Datasheet](https://www.silabs.com/documents/public/data-sheets/efr32mg1-datasheet.pdf) |
| Manual          | [Manual](https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf)     |

### Module versions
There are currently two types of modules available. The older module is labeled
ICC-1 and the newer one ICC-A-1. The main difference, is that pin PF3 is now
used to enable the SPI NOR Flash, and not exposed anymore.

### Pinout
Pin 1 is on the top-left side with only 6 contacts.

|      | PIN | PIN |      |
|------|-----|-----|------|
|      |     | 16  | RES  |
|      |     | 15  | PF3  |
| PA0  | 1   | 14  | PF2  |
| PA1  | 2   | 13  | PF1  |
| PB12 | 3   | 12  | PF0  |
| PB13 | 4   | 11  | PC11 |
| GND  | 5   | 10  | PC10 |
| 3V3  | 6   | 9   | PB14 |
|      |     | 8   | PB15 |
|      |     | 7   | GND  |

**Note**: On the ICC-A-1 module, PF3 is not exposed anymore.

### Peripheral mapping
| Peripheral | Number  | Hardware        | Pins                              | Comments                                            |
|------------|---------|-----------------|-----------------------------------|-----------------------------------------------------|
| ADC        | 0       | ADC0            | CHAN0: internal temperature       | Ports are fixed, 14/16-bit resolution not supported |
| HWCRYPTO   | &mdash; | &mdash;         |                                   | AES128/AES256, SHA1, SHA256                         |
| RTT        | &mdash; | RTCC            |                                   | 1 Hz interval. Either RTT or RTC (see below)        |
| RTC        | &mdash; | RTCC            |                                   | 1 Hz interval. Either RTC or RTT (see below)        |
| SPI        | 0       | USART1          | MOSI: PD15, MISO: PD14, CLK: PD13 |                                                     |
| Timer      | 0       | TIMER0 + TIMER1 |                                   | TIMER0 is used as prescaler (must be adjacent)      |
|            | 1       | LETIMER0        |                                   |                                                     |
| UART       | 0       | USART0          | RX: PB15, TX: PB14                | Default STDIO output                                |
|            | 1       | LEUART0         | RX: PB15, TX: PB14                | Baud rate limited (see below)                       |

### User interface
| Peripheral | Mapped to | Pin  | Comments |
|------------|-----------|------|----------|
| LED        | LED0      | PB13 |          |
|            | LED1      | PA1  |          |

## Implementation Status
| Device           | ID         | Supported | Comments                                                       |
|------------------|------------|-----------|----------------------------------------------------------------|
| MCU              | EFR32MG1P  | yes       | Power modes supported                                          |
| Low-level driver | ADC        | yes       |                                                                |
|                  | Flash      | yes       |                                                                |
|                  | GPIO       | yes       | Interrupts are shared across pins (see reference manual)       |
|                  | HW Crypto  | yes       |                                                                |
|                  | I2C        | yes       |                                                                |
|                  | PWM        | yes       |                                                                |
|                  | RTCC       | yes       | As RTT or RTC                                                  |
|                  | SPI        | partially | Only master mode                                               |
|                  | Timer      | yes       |                                                                |
|                  | UART       | yes       | USART is shared with SPI. LEUART baud rate limited (see below) |
| SPI NOR Flash    | IS25LQ020B | yes       | 2MBit flash. Can be used with the MTD API.                     |

## Board configuration

### UART selection
By default the UART peripheral is used for the TX/RX pins, however the pinout
is compatible with LEUART also. You can switch from UART to LEUART by compiling
with `EFM32_USE_LEUART=1`.

**Note**: LEUART is not working on the ICC-1 module.

### Clock selection
There are several clock sources that are available for the different
peripherals. You are advised to read
[AN0004.1](https://www.silabs.com/documents/public/application-notes/an0004.1-efm32-cmu.pdf)
to get familiar with the different clocks.

| Source | Internal | Speed      | Comments                           |
|--------|----------|------------|------------------------------------|
| HFRCO  | Yes      | 19 MHz     | Enabled during startup, changeable |
| HFXO   | No       | 38.4 MHz   |                                    |
| LFRCO  | Yes      | 32.768 kHz |                                    |
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

You can override the branch's clock source by adding `CLOCK_LFA=source` to
your compiler defines, e.g. `CLOCK_LFA=cmuSelect_LFRCO`.

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
For using the Silicon Labs STK3600 starter kit we strongly recommend
the usage of the [GNU Tools for ARM Embedded Processors](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
toolchain.

## License information
Silicon Labs' EMLIB: zlib-style license (permits distribution of source).
