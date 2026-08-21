@defgroup    boards_stm32c0316-dk STM32C0316-DK
@ingroup     boards
@brief       Support for the STM32C0316-DK board.

## General information

The ST [STM32C0316-DK](https://www.st.com/en/evaluation-tools/stm32c0316-dk.html)
is an evaluation board supporting a ARM Cortex-M0 STM32C031C6 microcontroller
with 12KB of RAM and 32KB of ROM Flash.

## Pinout

<img src="stm32c0316-dk.svg" alt="Pinout for the STM32C0316-DK (from STM user manual UM2969, https://www.st.com/resource/en/user_manual/um2969-discovery-kit-with-stm32c031c6-mcu-stmicroelectronics.pdf, page 7)" width=25% />

## MCU

| MCU        | STM32C031C6         |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0+      |
| Vendor     | ST Microelectronics |
| RAM        | 12KiB               |
| Flash      | 32KiB               |
| Frequency  | up to 48MHz         |
| FPU        | no                  |
| Timers     | 12 (2x watchdog, 1 SysTick, 5x 16-bit) |
| ADCs       | 1x 12-bit (up to 19 channels) |
| UARTs      |  2                  |
| SPIs       |  1                  |
| I2Cs       |  1                  |
| RTC        |  1                  |
| Vcc        |  2.0V - 3.6V        |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32c031c6.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0490-stm32c0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/um2969-discovery-kit-with-stm32c031c6-mcu-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32C0316-DK is `stm32c0316-dk` and the default
programmer is OpenOCD. Please note that the STM32C0 series is only supported by
OpenOCD versions newer than 0.12.0.
