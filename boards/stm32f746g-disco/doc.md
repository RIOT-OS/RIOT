/**
 * @defgroup    boards_stm32f746g-disco STM32F746G-DISCO
 * @ingroup     boards
 * @brief       Support for the STM32F746G-DISCO board.

## Overview

The ST [STM32F746G-DISCO](https://www.st.com/en/evaluation-tools/32f746gdiscovery.html)
is an evaluation board supporting a ARM Cortex-M7 STM32F746NG microcontroller
with 340KB of RAM and 1MB of ROM Flash.

As main features, this board provides:
- a 4"3 RGB 480×272 color LCD-TFT with capacitive touch screen,
- an ethernet port
- 2 user USB ports (FS and HS)
- 2 user digital microphones
- 128M-bit external Quas-SPI flash
- 128M-bit external SDRAM

Current hardware support:

| Feature        | Support     |  Configuration     |
| ------------- | :---------------------: | --------------------- |
| UART |  X  | USART1 on PB7/PA9 (stdio) and USART6 on PC6/PC7    |
| SPI |  X  | SPI2 on PB15/PB14/PI1    |
| I2C |  X | I2C1 on PB8/PB9    |
| Ethernet | X |  |
| USB OTG FS | X |  |
| USB OTG HS | X |  |
| TFT LCD | X |  |
| Capacitive touch screen | X | |
| User microphones | - | |
| External Quad-SPI Flash | - | |
| External SDRAM | - | |

## Flashing the device

The STM32F746G-DISCO board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD.
The board can be flashed with:

```
make BOARD=stm32f746g-disco flash
```

and debug via GDB with
```
make BOARD=stm32f746g-disco debug
```

## Supported Toolchains

For using the STM32F746G-DISCO board we recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.

 */
