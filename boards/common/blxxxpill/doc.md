@defgroup    boards_common_blxxxpill Blackpill and Bluepill common code
@ingroup     boards_common
@brief       Support for cheap stm32f103c8 based boards such as bluepill and blackpill.

## Overview

The bluepill and the blackpill are STM32F103 based board which can be bought for
around 2€ on sides like AliExpress, eBay and others. Although the STM32F130C8
MCU nominally has 64 KiB flash, most [actually have 128 KiB flash][Flashsize].

There are also versions that only report to have 32 KiB, but actually have
64 KiB.

\warning Be aware that *a lot* of these boards have fake STM32 chips,
which may or may not work with RIOT.

## Pinout

![pinout](pinout-bluepill.svg)

## MCU

| MCU       | STM32F103C8 / STM32F103CB |
|:----------|:--------------------------|
| Family    | ARM Cortex-M3             |
| Vendor    | STMicroelectronics        |
| RAM       | 20 KiB                    |
| Flash     | 32 KiB / 64 KiB / 128 KiB |
| Frequency | up to 72 MHz              |
| Timer     | 3x 16-Bit                 |
| ADC       | 2x 12-bit, 10 channels    |
| UART      | 3                         |
| SPI       | 2                         |
| I2C       | 2                         |
| CAN       | 1                         |
| Vcc       | 2.0 to 3.6 V              |
| Datasheet | [Datasheet][Datasheet]    |

## Implementation Status

| ID    | Supported |
|:----- |:--------- |
| GPIO  | yes       |
| PWM   | yes       |
| UART  | yes       |
| ADC   | yes       |
| I2C   | yes       |
| SPI   | yes       |
| USB   | no        |
| Timer | yes       |
| CAN   | no        |

## Flashing

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32F030C8 based Bluepill is `bluepill-stm32f030c8` and
for the STM32030C8 based Blackpill `blackpill-stm32f103c8`.

Be aware the the boards come with either the `C8` or the `CB` version of the microcontroller,
the former has 64 KiB flash while the latter has 128 KiB.

For the `CB` variants, you have to use the board names `bluepill-stm32f030cb` and
`blackpill-stm32f103cb`, respectively.

## Connecting via Serial

The default UART port used is UART1, which uses pins A9 (TX) and A10 (RX).
To use it, configure your UART to operate at a symbol rate of 115200 baud,
1 stop bit and no parity (8N1).

## Using PWM

PWM is available at pins A8 to A11.

## Known Issues

### USB connector

The Micro-USB port is sometimes not soldered properly. Also, it is
usually equipped with an incorrect resistor. [This can be fixed multiple
ways][USB].

### Flashing fails

Please check whether the debugger is properly connected, including the hardware
reset signal pin.

| Pin on Debugger    | Pin on Blue Pill |
|:------------------ |:---------------- |
| SWDIO              | DIO              |
| SWCLK              | DCLK             |
| NRST               | R                |
| GND                | GND              |
| VDD (3.3V) (*)     | 3.3              |
| Target VCC (**)    | 3.3              |

- (*)  Most debuggers have a 3.3V supply voltage to power the board. You can
       alternatively power the board using the TTL-Adapter or via Micro-USB.
- (**) Some debuggers monitor the voltage of the target MCU. Their target VCC
       pin needs to be connected to one of the 3.3V pins of the boards.

Most very cheap SWD debuggers (especially those imported for about 2€ from far
east) do not have a Target VCC pin; they will still work just fine. On these
cheap debuggers the pin to send the reset signal to the board is often either
not present or defunct. You can work around this by pressing the reset button
when OpenOCD wants to connect to the Blue Pill. Hit the reset button again after
flashing in order to boot the newly flashed image.

## Where to buy

The board is sold under different names. On some sites it is called
`bluepill` or `blue pill`. On others you might find it by searching for
`stm32f103c8` or `stm32f103c8t6`.

## Further reading

- [Blue Pill Wiki](https://web.archive.org/web/20190428082446/http://wiki.stm32duino.com/index.php?title=Blue_Pill)
- [Maple Mini Wiki](https://web.archive.org/web/20190428082446/http://wiki.stm32duino.com/index.php?title=Maple_Mini#Clones)
- [STM32F103C8 Datasheet](http://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f1-series/stm32f103/stm32f103c8.html)

[Datasheet]: http://www.st.com/content/ccc/resource/technical/document/datasheet/33/d4/6f/1d/df/0b/4c/6d/CD00161566.pdf/files/CD00161566.pdf/jcr:content/translations/en.CD00161566.pdf
[Flashsize]:
https://web.archive.org/web/20190428082446/http://wiki.stm32duino.com/index.php?title=Blue_Pill#128_KB_flash_on_C8_version
[USB]:
https://web.archive.org/web/20190428082446/http://wiki.stm32duino.com/index.php?title=Blue_Pill#Hardware_installation
