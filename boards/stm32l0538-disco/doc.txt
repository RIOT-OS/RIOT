/**
 * @defgroup    boards_stm32l0538-disco STM32L0538-DISCO
 * @ingroup     boards
 * @brief       Support for the STM32L0538-DISCO board

### Introduction

The
[STM32L0538-DISCO](https://www.st.com/en/evaluation-tools/32l0538discovery.html)
discovery kit features an ultra low-power stm32l053c8t6 microcontroller with
64KB of FLASH and 8KB of RAM.
The board also provides an on-board 2.04" E-paper display (not supported yet).

![STM32L0538-DISCO](https://www.st.com/content/ccc/fragment/product_related/rpn_information/board_photo/group0/67/a2/3f/98/6b/24/4a/27/stm32l0538-discovery.jpg/files/stm32l0538-disco.jpg/_jcr_content/translations/en.stm32l0538-disco.jpg)

### Supported features

| Peripheral            | Configuration                                                                             |
|:--------------------- |:----------------------------------------------------------------------------------------- |
| TIMs                  | TIM2                                                                                      |
| UARTs                 | USART1 on PA10 (RX), PA9 (TX)                                                             |
| SPIs                  | SPI1 on PB5 (MOSI), PB4 (MISO), PB3 (SCLK); SPI2 on PB15 (MOSI), PB14 (MISO), PB13 (SCLK) |

### Flashing the board

The board can be flashed using OpenOCD via the on-board ST-Link adapter.
Then use the following command:

    make BOARD=stm32l0538-disco -C examples/hello-world flash

### STDIO

STDIO is connected to pins PA9 (TX) and PA10 (RX) so an USB to UART adapter is
required. Use the `term` target to open a terminal:

    make BOARD=stm32l0538-disco -C examples/hello-world term

If an external ST-Link adapter is used, RX and TX pins can be directly connected
to it. In this case, STDIO is available on /dev/ttyACMx (Linux case).

 */
