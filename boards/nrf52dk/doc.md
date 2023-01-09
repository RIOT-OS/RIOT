/**
@defgroup    boards_nrf52dk nRF52 DK
@ingroup     boards
@brief       Support for the nRF52 DK

## Overview:

The
[nRF52 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF52-DK)
is a development board featuring the nRF52832 MCU with Arduino compatible
pin headers and an integrated J-Link programmer, debugger and UART adapter.

## Hardware:

![nRF52 DK](https://www.nordicsemi.com/-/media/Images/Products/DevKits/nRF52-Series/nRF52-DK/nRF52-DK/nRF52-DK-prod-page.png)

| MCU                   | NRF52832                                                                              |
|:--------------------- |:------------------------------------------------------------------------------------- |
| Family                | ARM Cortex-M4F                                                                        |
| Vendor                | Nordic Semiconductor                                                                  |
| RAM                   | 64 KiB                                                                                |
| Flash                 | 512 KiB                                                                               |
| Frequency             | 64 MHz                                                                                |
| FPU                   | yes                                                                                   |
| Timers                | 5 (32-bit)                                                                            |
| RTC                   | 3                                                                                     |
| ADCs                  | 1x 12-bit (8 channels)                                                                |
| UARTs                 | 1                                                                                     |
| SPIs                  | 3                                                                                     |
| I2Cs                  | 2                                                                                     |
| I2S                   | 1                                                                                     |
| PWM                   | 3*4 Channels                                                                          |
| Radio                 | 2.4GHz BLE compatiple, -20 dBm to +4 dBm output, -96 dBm RX sensitivity               |
| Vcc                   | 1.7V - 3.6V                                                                           |
| MCU Datasheet         | [Datasheet](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF52832)    |
| MCU Reference Manual  | [Reference Manual](https://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.4.pdf)        |
| Board Datasheet       | [User Guide](https://infocenter.nordicsemi.com/pdf/nRF52_DK_User_Guide_v1.3.1.pdf)    |

### RESET pin configuration

On many (all?) nrf52dk boards, the reset pin is not configured out-of-the box.
This means, that simply nothing happens if the RESET pin is pressed. To change
this, RIOT provides a little tool in `dist/tools/nrf52_resetpin_cfg`.

Simply compile, flash, and run that tool on your board, and the reset pin should
work for the time being.

## Current measurement:

There are two pins for current measurement on board. Don't connect these pins
to GND!. The left pin is at 3.3V level and the right pin is connected to V33.
Divide the measured voltage between this pins by 22 to get the current.

## Flash the board

See the `Flashing` section in @ref boards_common_nrf52.

## Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=nrf52dk -C examples/hello-world term
```
 */
