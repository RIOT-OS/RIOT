@defgroup    boards_pro-micro-nrf52840 Pro Micro nRF52840
@ingroup     boards
@brief       Support for Pro Micro nRF52840-compatible development boards

## Overview

Pro Micro nRF52840 boards are Arduino Pro Micro-compatible development boards
based on the nRF52840 SoC. This is a generic board definition that supports
multiple variants with compatible pinouts. There are many variants available,
including:

* Compatible:
  * Nice!Nano V2 (official, open-source)
  * SuperMini nRF52840
  * ProMicro nRF52840
  * Mikoto
  * 52840nano (more GPIO)

* Similar:
  * nRFMicro
  * BlueMicro

All these boards are largely the same in terms of functionality. They vary
slightly in pinouts, LED colors, battery charging capabilities and antenna
design. Though the board definition is generic, it follows the most common
pinout of the Nice!Nano V2.

This board definition is frequently used for building battery-powered custom
wireless keyboards. A comprehensive overview of various compatible variants,
including their specific features and known issues, can be found at
[NRF Micro Wiki - Alternatives](https://github.com/joric/nrfmicro/wiki/Alternatives).

## Hardware

The board is pin-compatible with the Arduino Pro Micro and features:

- nRF52840 SoC with ARM Cortex-M4F CPU
- 1 MiB Flash, 256 KiB RAM
- Bluetooth Low Energy 5.0
- USB-C connector with USB device support (some variants use USB Micro)
- Single board-provided LED (color varies by variant: blue, red, or green)
- Arduino Pro Micro form factor

### Pin Layout

The board follows the Pro Micro pinout. The following tables show the complete pin mapping:

#### Complete Pin Mapping

| nRF52840 Pin | Arduino Pin | Function | Notes           |
|--------------|-------------|----------|-----------------|
| P0.08        | D0          | UART RX  | Serial receive  |
| P0.06        | D1          | UART TX  | Serial transmit |
| P0.17        | D2          | I2C SDA  | I2C0 data line  |
| P0.20        | D3          | I2C SCL  | I2C0 clock      |
| P0.22        | D4          | GPIO     |                 |
| P0.24        | D5          | GPIO     |                 |
| P1.00        | D6          | GPIO     |                 |
| P0.11        | D7          | GPIO     |                 |
| P1.04        | D8          | GPIO     |                 |
| P1.06        | D9          | GPIO     |                 |
| P0.09        | D10         | GPIO     |                 |
| -            | D11         | -        | Not available   |
| -            | D12         | -        | Not available   |
| P0.15        | D13         | GPIO     | LED             |
| P1.11        | D14         | SPI MISO | SPI0 MISO       |
| P1.13        | D15         | SPI SCK  | SPI0 SCK        |
| P0.10        | D16         | SPI MOSI | SPI0 MOSI       |
| -            | D17         | -        | Not available   |
| P1.15        | D18         | GPIO     |                 |
| P0.02        | D19         | GPIO     |                 |
| P0.29        | D20         | GPIO     |                 |
| P0.31        | D21         | GPIO     |                 |

#### Primary Peripherals

| Peripheral    | nRF52840 Pins       | Arduino Pins  | Description            |
|---------------|---------------------|---------------|------------------------|
| UART (USB)    |                     |               | Used for STDIO         |
| UART (Serial) | P0.08, P0.06        | D0, D1        | RX, TX                 |
| I2C0          | P0.17, P0.20        | D2, D3        | SDA, SCL               |
| SPI0          | P0.10, P1.11, P1.13 | D16, D14, D15 | MOSI, MISO, SCK        |

The color of the LED may vary between board variants.

## Flashing, Bootloader, and Terminal

The board uses the Adafruit nRF52 Bootloader. Refer to the
[Adafruit nRF52 Bootloader](@ref boards_common_adafruit-nrf52-bootloader)
documentation for further details on the flashing process.

Example with `hello-world` application:

```shell
make BOARD=pro-micro-nrf52840 -C examples/basic/hello-world flash term
```

The terminal will connect to STDIO over USB (USB-CDC).
