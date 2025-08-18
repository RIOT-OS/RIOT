@defgroup   boards_sltb001a Silicon Labs SLTB001A starter kit
@ingroup    boards
@brief      Support for Silicon Labs SLTB001A starter kit

## Overview
Silicon Labs Thunderboard Sense is equipped with the EFM32 microcontroller. It
is specifically designed for low-power applications, having energy-saving
peripherals, different energy modes and short wake-up times.

The starter kit is equipped with an Advanced Energy Monitor. This allows you to
actively measure the power consumption of your hardware and code, in real-time.

## Hardware

### MCU
| MCU             | EFR32MG1P132F256GM48                                                                           |
|-----------------|------------------------------------------------------------------------------------------------|
| Family          | ARM Cortex-M4F                                                                                 |
| Vendor          | Silicon Labs                                                                                   |
| Vendor Family   | EFR32 Mighty Gecko 1P                                                                          |
| RAM             | 31.0 KiB                                                                                       |
| Flash           | 256.0 KiB                                                                                      |
| EEPROM          | no                                                                                             |
| Frequency       | up to 38.4 MHz                                                                                 |
| FPU             | yes                                                                                            |
| MPU             | yes                                                                                            |
| DMA             | 12 channels                                                                                    |
| Timers          | 2x 16-bit + 1x 16-bit (low power)                                                              |
| ADCs            | 12-bit ADC                                                                                     |
| UARTs           | 3x UART, 2x USART, 1x LEUART                                                                   |
| SPIs            | 2x USART                                                                                       |
| I2Cs            | 1x                                                                                             |
| Vcc             | 1.85 V - 3.8 V                                                                                 |
| Datasheet       | [Datasheet](https://www.silabs.com/documents/public/data-sheets/efr32mg1-datasheet.pdf)        |
| Manual          | [Manual](https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf)            |
| Board Manual    | [Board Manual](https://www.silabs.com/documents/public/user-guides/ug250-tb001-user-guide.pdf) |
| Board Schematic | Can be downloaded using Silicon Labs' Simplicity Studio                                        |

### Pinout
This is the pinout of the expansion pins on the front side of the board. PIN 1
is the top-left contact, marked on the silkscreen.

|      | PIN | PIN |      |
|------|-----|-----|------|
| GND  | 1   | 2   | VMCU |
| PA2  | 3   | 4   | PC6  |
| PA3  | 5   | 6   | PC7  |
| PF3  | 7   | 8   | PC8  |
| PF4  | 9   | 10  | PC9  |
| PF5  | 11  | 12  | PC0  |
| PF6  | 13  | 14  | PC1  |
| PC11 | 15  | 16  | PC10 |
| RES  | 17  | 18  | 5V   |
| RES  | 19  | 20  | 3V3  |

**Note**: not all starter kits by Silicon Labs share the same pinout!

### Peripheral mapping
| Peripheral | Number  | Hardware        | Pins                           | Comments                                                 |
|------------|---------|-----------------|--------------------------------|----------------------------------------------------------|
| ADC        | 0       | ADC0            | CHAN0: internal temperature    | Ports are fixed, 14/16-bit resolution not supported      |
| I2C        | 0       | I2C0            | SDA: PC10, CLK: PC11           | `I2C_SPEED_LOW` and `I2C_SPEED_HIGH` clock speed deviate |
| HWCRYPTO   | &mdash; | &mdash;         |                                | AES128/AES256, SHA1, SHA256                              |
| RTT        | &mdash; | RTCC            |                                | 1 Hz interval. Either RTT or RTC (see below)             |
| RTC        | &mdash; | RTCC            |                                | 1 Hz interval. Either RTC or RTT (see below)             |
| SPI        | 0       | USART1          | MOSI: PC6, MISO: PC7, CLK: PC8 |                                                          |
| Timer      | 0       | TIMER0 + TIMER1 |                                | TIMER0 is used as prescaler (must be adjacent)           |
|            | 1       | LETIMER0        |                                |                                                          |
| UART       | 0       | USART0          | RX: PA1, TX: PA0               | Default STDIO output                                     |
|            | 1       | LEUART0         | RX: PD11, TX: PD10             | Baud rate limited (see below)                            |

### User interface
| Peripheral | Mapped to | Pin  | Comments  |
|------------|-----------|------|-----------|
| Button     | PB0       | PD14 |           |
|            | PB1       | PD15 |           |
| LED        | LED0      | PD11 | Red LED   |
|            | LED1      | PD12 | Green LED |

## Implementation Status
| Device                        | ID        | Supported | Comments                                                       |
|-------------------------------|-----------|-----------|----------------------------------------------------------------|
| MCU                           | EFR32MG1P | yes       | Power modes supported                                          |
| Low-level driver              | ADC       | yes       |                                                                |
|                               | Flash     | yes       |                                                                |
|                               | GPIO      | yes       | Interrupts are shared across pins (see reference manual)       |
|                               | HW Crypto | yes       |                                                                |
|                               | I2C       | yes       |                                                                |
|                               | PWM       | yes       |                                                                |
|                               | RTCC      | yes       | As RTT or RTC                                                  |
|                               | SPI       | partially | Only master mode                                               |
|                               | Timer     | yes       |                                                                |
|                               | UART      | yes       | USART is shared with SPI. LEUART baud rate limited (see below) |
|                               | USB       | no        |                                                                |
| Temperature + humidity sensor | Si7021    | yes       | Silicon Labs Temperature + Humidity sensor                     |
| Microphone                    |           | no        |                                                                |
| Pressure + temperature sensor | BMP280    | yes       | Bosch pressure and temperature sensor                          |
| Light sensor                  | Si1133    | no        | Silicon Labs UV/Ambient Light sensor                           |
| Hall-effect sensor            | Si7210    | no        | Silicon Labs Hall-effect sensor (Rev. A02 boards only)         |
| IMU sensor                    | ICM-20648 | no        | InvenSense 6-axis inertial sensor                              |
| Air sensor                    | CCS811    | yes       | Cambridge CMOS Sensors Air Quality/Gas sensor                  |

Most sensors on this board are controlled via a power and interrupt controller
(PIC). By default, this module is enabled. You can disable this module by
adding `DISABLE_MODULE=silabs_pic` to the `make` command.

## Board configuration

### Board controller
The starter kit is equipped with a Board Controller. This controller provides
a virtual serial port.

**Note:** the board controller *always* configures the virtual serial port at
115200 baud with 8 bits, no parity and one stop bit. This also means that it
expects data from the MCU with the same settings.

### Clock selection
There are several clock sources that are available for the different
peripherals. You are advised to read [AN0004.0](https://www.silabs.com/documents/public/application-notes/an0004.0-efm32-cmu.pdf)
to get familiar with the different clocks.

| Source | Internal | Speed      | Comments                           |
|--------|----------|------------|------------------------------------|
| HFRCO  | Yes      | 19 MHz     | Enabled during startup, changeable |
| HFXO   | No       | 38.4 MHz   |                                    |
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
For using the Silicon Labs SLTB001A starter kit we strongly recommend
the usage of the [GNU Tools for ARM Embedded Processors](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
toolchain.

## License information
Silicon Labs' EMLIB: zlib-style license (permits distribution of source).
