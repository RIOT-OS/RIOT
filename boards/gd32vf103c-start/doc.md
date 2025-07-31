@defgroup   boards_gd32vf103c_start GD32VF103C-START
@ingroup    boards
@brief      Support for the GD32VF103C-START board
@author     Marian Buschsieweke

## Overview

The GD32VF103C-START development board is an official starter kit by GigaDevice
for the GD32VF103CBT6 MCU, despite the lack of official documentation. It
featrures:

- GD32VF103CBT6 RISC-V MCU @108MHz
- On-Board GD-Link Programmer/Debugger (middle mini-USB connector)
- USB-OTG (left mini-USB connector)
- Integrated USB to UART bridge (right mini-USB connector)
- 1 user LED
- 1 user Button
- 1 reset button
- Arduino UNO compatible pin headers (except ISP header)

![Image of the GD32VF103C-START board](https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/a0ac2faf9bde45b176f1e8b315ffb1b8a9096166/boards/riscv/gd32vf103c_starter/doc/img/gd32vf103c_starter.jpg)

@warning    The analog pins are labeled A1 - A6 rather than A0 - A5, as would
            be the correct Arduino naming. The Arduino pin mapping uses the
            correct Arduino naming, so that apps written for other boards
            expecting Arduino UNO compatible shields remain compatible.

## Hardware

| MCU               | GD32VF103CBT6                             | Supported |
|:----------------- |:----------------------------------------- | --------- |
| Family            | RISC-V with ECLIC                         |           |
| Vendor            | GigaDevice                                |           |
| RAM               | 32 KiB                                    |           |
| Flash             | 128 KiB                                   |           |
| Frequency         | 108 MHz                                   |           |
| Power Modes       | 3 (Sleep, Deep Sleep, Standby)            |   yes     |
| GPIOs             | 37                                        |   yes     |
| Timers            | 5 x 16-bit timer                          |   yes     |
| RTC               | 1 x 32-bit counter, 20-bit prescaler      |   yes     |
| WDT               | 2 x 12-bit counter, 3-bit prescaler       |   yes     |
| ADC               | 2 x 12-bit units, 16 channels @ 1 Msps    |   yes     |
| DAC               | 2 x 12-bit channel                        |   yes     |
| UART              | -                                         |   yes     |
| USART             | 3                                         |   yes     |
| SPI               | 3                                         |   yes     |
| I2C               | 2 x Fast Mode 400 kHz                     |   yes     |
| I2S               | 2                                         |   no      |
| CAN               | 2 x CAN 2.0B with up to 1 Mbps            |   no      |
| PWM               | 6 Channels                                |   yes     |
| USB               | 1 x USB FS OTG (+ GD-Link + UART bridge)  |   yes     |
| Vcc               | 3.0V - 3.6V                               |           |
| Datasheet         | [Datasheet][gd32vf103c-datasheet]         |           |
| User Manual       | [User Manual][gd32vf103c-manual]          |           |
| Board Manual      | [Board Manual][gd32vf103c-board-manual]   |           |

[gd32vf103c-datasheet]: https://web.archive.org/web/20240117072854/https://gd32mcu.com/data/documents/datasheet/GD32VF103_Datasheet_Rev1.6.pdf
[gd32vf103c-manual]: https://web.archive.org/web/20240117073025/https://gd32mcu.com/data/documents/userManual/GD32VF103_User_Manual_Rev1.5.pdf
[gd32vf103c-board-manual]: https://web.archive.org/web/20240117072723/https://en.maritex.com.pl/product/attachment/154626/6f28fdabafb9dff852537ced7bc0aa71

## Flashing

@warning    Flashing seems to fail with some Arduino UNO compatible boards
            attached. Remove them and try again.

By default, flashing is done via OpenOCD using the GD-Link programmer/debugger
using:

```
make BOARD=gd32vf103c-start -C path/to/app flash
```

It is also possible to flash via DFU-Util:

1. Connect a mini USB cable to the USB connector labeled OTG.
2. Power the board (e.g. by connecting a second mini USB cable to the GD-Link
   programmer/debugger, even though we won't use it)
3. Enter the DFU bootloader by placing the jumper on JP4 (below BOOT) in
   position H. Press the reset button afterwards.
4. Run `make BOARD=gd32vf103c-start PROGRAMMER=dfu-util -C path/to/app flash`
5. Restore the JP4 jumper to position L. Afterwards press the reset button
   again

## Connecting to the Serial Output

By default RIOT's uses UART for `stdio`. To access that, connect a mini USB
cable to the connector labeled "USART". In addition a jumper needs to be
placed on JP2 in the "Usart" position; otherwise RIOT's TXD is not connected
to the RXD of the integrated USB UART bridge. Afterwards just run:

```
make BOARD=gd32vf103c-start -C path/to/app term
```

Alternatively, `stdio_cdc_acm` can be used. In this case the mini USB cable
needs to be connected to the USB connector labeled OTG.

@note   It is safe to keep the jumper at JP2 in "Usart" position or even
        remove the jumper with RIOT. PA9 is still used as UART/TXD and VBUS
        sensing is disabled anyway.
