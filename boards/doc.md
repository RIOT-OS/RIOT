/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards Boards
 * @brief       Board specific definitions and implementations
 *
 * The boards module contains all definitions and implementations that are
 * specific to a certain board. Generally, boards consist of a fixed
 * configuration of a controller and some external devices such as sensors or
 * radios. All aspects concerning configuration of GPIO pins, MCU clock and
 * device drivers should go into this module.
 *

Guide to board pinouts {#pinout_guide}
=====================================

The following are the steps to take to locate specific pinouts on boards.
This is useful when connecting external hardware to RIOT supported boards.

1. Check the [board documentation](http://doc.riot-os.org/group__boards.html)
to see if the pinout is available
2. If the pinout is not available or up to date check the following:
    - `boards/<BOARD>/include/periph_conf.h` for board specific
peripherals such as UART or SPI pins
    - `boards/<BOARD>/include/board.h` for GPIO specific pins such
as `LED0` or `BTN0`
    - `boards/common/<COMMON_BOARD_OR_CPU>/include/periph_conf_common.h`
for shared pins that are the same for many different boards (board_common.h
or others will be available too)
    - `boards/common/<COMMON_BOARD_OR_CPU>/include/<PERIPH_DECLARATION>`
also may contain specific periphirals such as
`PERIPH_DECLARATION = cfg_i2c1_pb8_pb9.h` which indicates I2C pins are on
pb8 and pb9
3. Once the needed pins are determined with either a port where
`PORTA = PORT_A = PA = 0` or `PORTB = PORT_B = PB = 1` and pin
number, such as `PA, 1`, correlate that to the board (the board
may either have the designators or search online to find which GPIO
corresponds to physical location on the board)
4. Match the `GPIO` with the specific pin and connect the wires

### Examples of finding pinouts
The following will be different examples of how to find pinouts

#### Finding pins of the I2C device 0 on samr21-xpro
1. The documentation says I2C is supported but does not say the pinout
2. Checking the pinout in `boards/samr21-xpro/include/periph_conf.h` the
`scl_pin = GPIO_PIN(PA, 17)` and the `sda_pin = GPIO_PIN(PA, 16)`
3. Since the board has the GPIO port and pin printed on the board the
physical location is `PA16` and `PA17`

#### Finding pins of the UART device 1 on nucleo-f103rb
_The dev number is only the location in the array and does not mean the
number of the peripheral_
1. The documentation says UART is supported but does not say the pinout
2. Checking the pinout in `boards/nucleo-f103rb/include/periph_conf.h` the
`rx_pin = GPIO_PIN(PORT_A, 10)`, the `tx_pin = GPIO_PIN(PORT_A, 9)` and the
`dev = USART1`, though device 1 can equal any USARTn number
3. Since the board does not have information on where these pins are
physically located it is recommended to search
[online](https://os.mbed.com/platforms/ST-Nucleo-F103RB/) for that board
4. After locating the physical layout the rx_pin can be found on CN9-1
using either `PA_10` or `UART1_RX` and the tx_pin can be found on CN5-1
using either `PA_9` or `UART1_TX`, it is recommended to use the GPIO pin as
sometimes the peripheral may be using an alternate pin
 */
