@defgroup    boards Boards
@brief       Board specific definitions and implementations

The boards module contains all definitions and implementations that are
specific to a certain board. Generally, boards consist of a fixed
configuration of a controller and some external devices such as sensors or
radios. All aspects concerning configuration of GPIO pins, MCU clock and
device drivers should go into this module.


Board Selection Guide {#board-selection-guide}
=====================

There is no single best board to buy for use with RIOT, as hardware design
involves many trade-offs. The challenge is to clearly assess the use case and
collect the requirements and to find the best match for those.

With this in mind, it is still possible to give some hints and recommendations:

1. During Development:
    - An integrated programmer/debugger is immensely useful. At the very least
      a standard JTAG/SWD connector for quick and fool-proof connection is
      required
    - If possible, pick an MCU with *more* RAM and flash than your target board.
      This allows to spend the excess in memory for debug features such as
      utility modules, verbose assert messages, etc. during development
    - Standard connectors such as Arduino Headers, or Grove, STEMMA, STEMMA QT,
      QUIIC connectors etc. make hardware prototyping easier
2. Final Product:
    - An integrated programmer/debugger is a waste of resources (money, power,
      space, ...)
    - ...

Popular Boards
--------------

| Board                                 | Use Case              | Performance   | Integrated Debugger   | Integrated Networking             | Native USB    | Arduino Headers       | Other Connectors          | Integrated Sensors/...                                | Breadboard Friendly   | Costs     | Remarks                       |
|:------------------------------------- |:--------------------- |:------------- |:--------------------- |:--------------------------------- |:------------- |:--------------------- |:------------------------- |:----------------------------------------------------- |:--------------------- |:--------- |:----------------------------- |
| @ref boards_arduino-due               | Education             | `++`          | ✔                     | ✗                                 | ✔             | `+++` (Uno, Mega, ISP)| ✗                         | `-` (1 LED)                                           | `+`                   | `o`       | Better buy the nrf52840dk     |
| @ref boards_arduino-mega2560          | You have them anyway  | `--`          | ✗                     | ✗                                 | ✗             | `+++` (Uno, Mega, ISP)| ✗                         | `-` (1 LED)                                           | `+`                   | `--`      | Better buy the nrf52840dk     |
| @ref boards_arduino-uno               | You have them anyway  | `---`         | ✗                     | ✗                                 | ✗             | `++` (Uno, ISP)       | ✗                         | `-` (1 LED)                                           | `+`                   | `--`      | You like pain, don't you?     |
| @ref boards_common_nucleo32           | Development           | `-/o/+/++`    | ✔                     | ✗                                 | ✗             | `o` (Nano)            | Custom                    | `o` (1 button, 1 LED)                                 | `+`                   | `++`      | Good bang for the buck        |
| @ref boards_common_nucleo64           | Development           | `o/+/++`      | ✔                     | ✗                                 | ✗             | `+` (Uno)             | ST Morpho Headers         | `o` (1 button, 1 LED)                                 | `+`                   | `++`      | Good bang for the buck        |
| @ref boards_common_nucleo144          | Development           | `+/++/+++`    | ✔                     | (✔) (some have Ethernet)          | ✔             | `++` (Uno, Mega)      | ST Morpho Headers         | `+` (1 button, 3 LEDs)                                | `+`                   | `++`      | Good bang for the buck        |
| @ref boards_esp32_esp-ethernet-kit    | Development           | `+++`         | ✔                     | ✔ (WiFi, BLE, Ethernet, custom)   | ✗             | ✗                     | Custom                    | `-` (1 button)                                        | `+`                   | `o`       | Requires proprietary software |
| @ref boards_esp32_wroom-32            | Prototyping           | `+++`         | ✗                     | ✔ (WiFi, BLE, custom)             | ✗             | ✗                     | Custom                    | `-` (1 button)                                        | `+++`                 | `+++`     | Requires proprietary software |
| @ref boards_microbit_v2               | Education             | `++`          | ✔                     | ✔ (802.15.4, BLE, custom)         | ✗             | ✗                     | micro:bit edge connector  | `+++` (6 buttons, LED matrix, mic, speaker, IMO)      | `--`                  | `++`      | Good education board          |
| @ref boards_nrf52840dk                | Development           | `++`          | ✔                     | ✔ (802.15.4, BLE, custom)         | ✔             | `+++`(Uno, Mega)      | Custom                    | `+` (4 buttons, 4 LEDs)                               | `+`                   | `+`       | Good wireless dev board       |
| @ref boards_feather-nrf52840-sense    | Prototyping/Education | `++`          | ✗                     | ✔ (802.15.4, BLE, custom)         | ✔             | ✗                     | Adafruit Feather          | `++` (orientation, air parameters, light/gestures)    | `+++`                 | `+`       | used in [inetrg exercises]    |
| @ref boards_nrf52840dongle            | Prototyping           | `++`          | ✗                     | ✔ (802.15.4, BLE, custom)         | ✔             | ✗                     | Custom                    |  o  (1 button, 4 LEDs)                                | `++`                  | `++`      | Excellent border router       |
| @ref boards_nucleo-wl55jc             | Development           | `++`          | ✔                     | ✔ (LoRa)                          | ✗             | `+` (Uno)             | ST Morpho Headers         | `+` (3 buttons, 3 LEDs)                               | `+`                   | `++`      | Good bang for the buck        |
| @ref boards_pinetime                  | Gadget                | `++`          | ✗                     | ✔ (BLE)                           | ✗             | ✗                     | ✗                         | `+++` (LCD, button, touch screen, IMU, flash, ...)    | `---`                 | `+++`     | Buy two: One with SWD access  |
| @ref boards_samr21-xpro               | Development           | `+`           | ✔                     | ✔ (802.15.4)                      | ✔             | ✗                     | XPRO Expansion Header     | `o` (1 button, 1 LED)                                 | `+`                   | `--`      | Quite expensive               |
| @ref boards_samr34-xpro               | Development           | `++`          | ✔                     | ✔ (LoRa)                          | ✔             | ✗                     | XPRO Expansion Header     | `o` (1 button, 2 LEDs)                                | `+`                   | `---`     | Got a spare kidney to sell?   |
| @ref boards_weact-f411ce              | Prototyping           | `++`          | ✗                     | ✗                                 | ✔             | ✗                     | Custom                    | `+` (1 button, 1 LED, SPI flash)                      | `+++`                 | `+++`     | Excellent bang for the buck   |

@note       Only boards with mature RIOT support and decent documentation qualify for above list
@details    This list was last updated in April 2024


<!-- Add when doc is fixed
| @ref boards_b-l072z-lrwan1            | Development           | `+`           | ✔                     | ✔ (LoRa)                          | ✗             | `++` (Uno, Mega)      | ST Morpho Headers         | `+` (1 button, 4 LEDs)                                | `+`                   | `++`      | Good bang for the buck        |
-->

[inetrg exercises]: https://github.com/inetrg/exercises

### Notes on Arduino Compatibility

- Perfect (`+++`) only if it is fully compatible with both Arduino Uno and Arduino Mega Shields (including SPI via ISP connector)
- Good (`++`) if it is fully compatible with Arduino Uno shields (including SPI via ISP connector)
- Good (`++`) if it is partially compatible with both Arduino Uno and Arduino Mega Shields (no SPI via ISP connector)
- Decent (`+`) if it is partially compatible with Arduino Uno Shields (no SPI via ISP connector)
- Decent (`+`) if it is fully compatible with Arduino Nano Shields (including SPI via ISP connector)
- OK (`o`) if it is partially compatible with Arduino Nano Shields

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
