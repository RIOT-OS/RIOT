@defgroup    boards_saml11-xpro Microchip SAML11 Xplained Pro
@ingroup     boards
@brief       Support for the Microchip SAML11 Xplained Pro board.

## Overview

The `SAML11 Xplained Pro` is an ultra-low power evaluation board by Microchip
featuring a ATSAML11E16A SoC. The SoC includes a SAML11 ARM Cortex-M23 micro-
controller. For programming the MCU comes with 16KB of RAM and 64KB of flash
memory. In addition, this SoC features the ARM TrustZone technology.

## Hardware

![saml11-xpro image](https://www.microchip.com/_ImagedCopy/Converted-146683-1559149241-C16616-180424-MCU32-PHOTO-DM320205-Front-Transparent.png)


### MCU
| MCU        | ATSAML11E14A      |
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
| MCU        | saml11    | partly    | PLL clock not implemented |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | no            | |
|        | UART      | yes           | |
|        | I2C       | yes        | |
|        | SPI       | yes        | |
|        | USB       | no        | |
|        | RTT       | yes       | |
|        | RTC       | yes      |  |
|        | RNG       | yes        |  |
|        | Timer     | yes           | |
|        | ADC       | yes         | |



## Flashing the device

Connect the device to your Micro-USB cable.

The standard method for flashing RIOT to the saml11-xpro is using EDBG.

## Special case

SERCOM1 (available on EXT1 connector) needs an extra step to be usable.
By default, this SERCOM is only available in the secure world. As RIOT
doesn't support it for now, the only option, to use it, is to enable
SERCOM non-secure mode. To do so, a fuse bit must be set in User ROW
flash memory. Such action can be done with the following EDBG command:
'edbg -t saml11 -F w0,194,1'
or pass it as argument when calling make:
EDBG_ARGS="-F w0,194,1" BOARD=saml11-xpro make flash term -C tests/periph/uart
