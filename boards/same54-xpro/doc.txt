/**
@defgroup    boards_same54-xpro Microchip SAM E54 Xplained Pro
@ingroup     boards
@brief       Support for the Microchip SAM E54 Xplained Pro board.

## Overview

The `SAME54 Xplained Pro` is an evaluation board by Microchip featuring an
ATSAME54P20A SoC. The SoC includes a SAME54 ARM Cortex-M4F microcontroller.
For programming the MCU comes with 256Kb of RAM and 1024Kb of flash memory.

The same54-xpro is available from various hardware vendors for ~70USD (as of
2019 March).

## Hardware

![same54-xpro image](http://www.microchip.com/_ImagedCopy/ATSAME54-XPRO_angle.jpg)


### MCU
| MCU        | ATSAME54P20A      |
|:------------- |:--------------------- |
| Family | ARM Cortex-M4F    |
| Vendor | Microchip |
| RAM        | 256Kb |
| Flash      | 1024Kb             |
| Frequency  | up to 120MHz |
| FPU        | yes                |
| Timers | 8 (16-bit)    |
| ADCs       | 2x 12-bit (20 channels)           |
| UARTs      | max 8 (shared with SPI and I2C)               |
| SPIs       | max 8 (see UART)                  |
| I2Cs       | max 8 (see UART)              |
| Vcc        | 1.62V - 3.63V         |
| Datasheet  | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/60001507C.pdf) |
| Board Manual   | [Board Manual](http://ww1.microchip.com/downloads/en/DeviceDoc/70005321A.pdf)|

### User Interface

1 User button and 1 LED:

| Device | PIN |
|:------ |:--- |
| LED0   | PC18 |
| SW0 (button) | PB31 |


## Implementation Status

| Device           | ID        | Supported | Comments  |
|:---------------- |:--------- |:--------- |:--------- |
| MCU              | same54    | partly    |           |
| Low-level driver | GPIO      | yes       |           |
|                  | ADC       | yes       |           |
|                  | CAN       | no        |           |
|                  | DAC       | yes       |           |
|                  | QSPI      | no        | see #15300|
|                  | SD/MMC    | no        |           |
|                  | PWM       | yes       |           |
|                  | UART      | yes       |           |
|                  | I2C       | yes       |           |
|                  | SPI       | yes       |           |
|                  | USB       | yes       |           |
|                  | RTT       | yes       |           |
|                  | RTC       | yes       |           |
|                  | RNG       | yes       |           |
|                  | Timer     | yes       |           |
|                  | Ethernet  | yes       |           |


## Flashing the device

Connect the device to your Micro-USB cable using the port labeled as *DEBUG
USB*.

The standard method for flashing RIOT to the same54-xpro is using EDBG.

On Linux you will have to add a **udev** rule for hidraw, like
```
bash
echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"' \
    | sudo tee -a /etc/udev/rules.d/99-usb.rules
sudo service udev restart
```

## Supported Toolchains

For using the same54-xpro board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.

## Known Issues / Problems

 */
