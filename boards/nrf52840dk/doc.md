@defgroup    boards_nrf52840dk nRF52840 DK
@ingroup     boards
@brief       Support for the nRF52840 DK

### General information

The [nRF52840 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF52840-DK)
is a devboard based on the nRF52840 SoC which supports multiple wireless protocols
including Bluetooth Low Energy, Thread and Zigbee.

## Hardware

| MCU                   | nRF52840                                                                                                          |
|:--------------------- |:------------------------------------------------------------------------------------------------------------------|
| Family                | ARM Cortex-M4                                                                                                     |
| Vendor                | Nordic Semiconductor                                                                                              |
| RAM                   | 256 KiB                                                                                                           |
| Flash                 | 1 MiB                                                                                                             |
| Frequency             | 64 MHz                                                                                                            |
| FPU                   | yes                                                                                                               |
| Timers                | 5 (32-bit)                                                                                                        |
| RTC                   | 3                                                                                                                 |
| ADCs                  | 1x 12-bit (8 channels)                                                                                            |
| UARTs                 | 2                                                                                                                 |
| SPIs                  | 4 masters/3 slaves                                                                                                |
| I2Cs                  | 2                                                                                                                 |
| I2S                   | 1                                                                                                                 |
| PWM                   | 4*4 Channels                                                                                                      |
| Radio                 | 2.4GHz BLE compatible, -20 dBm to +8 dBm output, -95 dBm RX sensitivity                                           |
| Vcc                   | 1.7V - 5.5V                                                                                                       |
| MCU Manual            | [Manual](https://docs-be.nordicsemi.com/bundle/ps_nrf52840/attach/nRF52840_PS_v1.11.pdf)                          |
| Board Documentation   | [Documentation](https://docs.nordicsemi.com/bundle/ug_nrf52840_dk/page/UG/dk/intro.html)                          |

### RESET pin configuration

On many (all?) nrf52840dk boards, the reset pin is not configured out-of-the box.
This means, that simply nothing happens if the RESET pin is pressed. To change
this, RIOT provides a little tool in `dist/tools/nrf52_resetpin_cfg`.

Simply compile, flash, and run that tool on your board, and the reset pin should
work for the time being.

## Flash the board

See the `Flashing` section in @ref boards_common_nrf52.

## Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=nrf52840dk -C examples/basic/hello-world term
```
