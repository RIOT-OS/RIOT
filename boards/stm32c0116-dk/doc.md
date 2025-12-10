@defgroup    boards_stm32c0116-dk STM32C0116-DK
@ingroup     boards
@brief       Support for the STM32C0116-DK board.

## General information

The ST [STM32C0116-DK](https://www.st.com/en/evaluation-tools/stm32c0116-dk.html)
is an evaluation board supporting a ARM Cortex-M0 STM32C011F6 microcontroller
with 8KB of RAM and 32KB of ROM Flash.

## Pinout

See [this application note as reference](https://www.st.com/resource/en/application_note/an5673-getting-started-with-stm32c0-mcu-hardware-development-stmicroelectronics.pdf).
This means the responsibility is on the firmware configurer to take special
care when configuring IO, ensuring that ports are not conflicting on each pin.

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32C0116-DK is `stm32c0116-dk` and the default
programmer is OpenOCD. Please note that the STM32C0 series is only supported by
OpenOCD versions newer than 0.12.0.
