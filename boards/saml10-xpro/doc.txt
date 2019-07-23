/**
@defgroup    boards_saml10-xpro Microchip SAML10 Xplained Pro
@ingroup     boards
@brief       Support for the Microchip SAML10 Xplained Pro board.

## Overview

The `SAML10 Xplained Pro` is an ultra-low power evaluation board by Microchip
featuring a ATSAML10E16A SoC. The SoC includes a SAML10 ARM Cortex-M23 micro-
controller. For programming the MCU comes with 16KB of RAM and 64KB of flash
memory.

## Hardware

![saml10-xpro image](https://www.microchip.com/_ImagedCopy/SAML10%20Xpro%20Front%20Title.jpg)


### MCU
| MCU        | ATSAML10E16A      |
|:------------- |:--------------------- |
| Family | ARM Cortex-M23    |
| Vendor | Microchip |
| RAM        | 16KB |
| Flash      | 64KB             |
| Frequency  | up to 32MHz |
| FPU        | no                |
| Timers | 3 (16-bit)    |
| ADCs       | 1x 12-bit (10 channels)           |
| UARTs      | max 3 (shared with SPI and I2C)               |
| SPIs       | max 3 (see UART)                  |
| I2Cs       | max 3 (see UART)              |
| Vcc        | 1.6V - 3.6V           |
| Datasheet  | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/SAM-L10L11%20Family-DataSheet%20-%20DS60001513B.pdf) |
| Board Manual   | [Board Manual](http://ww1.microchip.com/downloads/en/DeviceDoc/70005359B.pdf)|

### User Interface

1 User button and 1 LED:

| Device | PIN |
|:------ |:--- |
| LED0   | PA07 |
| SW0 (button) | PA27 |


## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | saml10    | partly    | PLL clock not implemented |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | no            | |
|        | UART      | yes           | |
|        | I2C       | yes        | |
|        | SPI       | yes        | |
|        | USB       | no        | |
|        | RTT       | yes        | |
|        | RTC       | yes      |  |
|        | RNG       | yes        |  |
|        | Timer     | yes           | |
|        | ADC       | yes         | |

## Flashing the device

Connect the device to your Micro-USB cable.

The standard method for flashing RIOT to the saml10-xpro is using EDBG.

## Supported Toolchains

For using the saml10-xpro board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.

 */
