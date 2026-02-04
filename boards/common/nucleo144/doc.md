@defgroup    boards_common_nucleo144 STM32 Nucleo-144
@ingroup     boards
@brief       Support for STM32 Nucleo-144 boards

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).

## Accessing the RIOT shell

The default RIOT OS shell access utilizes the ST-Link VCP (Virtual COM Port)
that is featured on every Nucleo board. The USART used for this connection
varies between the different Nucleo variants, so you have to consult the
`nucleo-xxxxx/include/periph_conf.h` header file for more information.

The default baud rate is 115200, 8N1.
