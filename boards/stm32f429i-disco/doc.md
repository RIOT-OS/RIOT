@defgroup    boards_stm32f429i-disco STM32F429I-DISCO
@ingroup     boards
@brief       Support for the STM32F429I-DISCO board

### General information

The board is identical to the @ref boards_stm32f429i-disc1 except that it uses
an older on-board ST-LINK debugger that does not provide a serial UART interface.

To mitigate that, stdio is instead provided through the micro-USB connector using
RIOT's CDC ACM functionality.

### MCU

| MCU          | STM32F429ZI         |
|:-------------|:--------------------|
| Family       | ARM Cortex-M4F      |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 2MiB                |
| Frequency    | up to 180 MHz (set to 180MHz in RIOT) |
| RIOT default | 180 MHz             |
| FPU          | yes                 |
| Ethernet     | 10/100 Mbps         |
| Timers       | 17 (2x watchdog, 1 SysTick, 2x 32bit, 12x 16bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 3                   |
| SPIs         | 6                   |
| CAN          | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f429zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1662-getting-started-with-the-stm32f429-discovery-kit-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32F429I-DISCO is `stm32f429i-disco`.