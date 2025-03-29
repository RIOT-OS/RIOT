@defgroup boards_common_nucleo64 STM32 Nucleo-64
@ingroup boards
@brief Support for STM32 Nucleo-64 Boards

## Using LED0 and SPI at once

Due to a pin conflict on Nucleo64 boards, you can not use
the SPI peripheral and LED0 at the same time.
A warning will be shown during compile time:
```
Warning: Using periph_spi on Nucleo64 boards will disable LED0 due to pin conflicts.
```

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).

## Accessing the RIOT shell

The default RIOT OS shell access utilizes the ST-Link VCP (Virtual COM Port)
that is featured on every Nucleo board. The USART used for this connection
varies between the different Nucleo variants, so you have to consult the
`nucleo-xxxxx/include/periph_conf.h` header file for more information.

The default baud rate is 115200, 8N1.
