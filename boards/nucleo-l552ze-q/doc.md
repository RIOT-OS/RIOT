/**
@defgroup    boards_nucleo-l552ze-q STM32 Nucleo-L552ZE-Q
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-L552ZE-Q

## Overview

The Nucleo-L552ZE-Q is a board from ST's Nucleo family supporting ARM Cortex-M33
STM32L552ZE ultra-low-pawer microcontroller with TrustZone, 256KiB or RAM and 512KiB
of Flash.

## Hardware

![Nucleo144 L552ZE-Q](https://www.st.com/bin/ecommerce/api/image.PF267817.en.feature-description-include-personalized-no-cpn-large.jpg)

### MCU

| MCU          | STM32L552ZE                  |
|:-------------|:-----------------------------|
| Family       | ARM Cortex-M33               |
| Vendor       | ST Microelectronics          |
| RAM          | 256KiB                       |
| Flash        | 512KiB                       |
| Frequency    | up tp 110MHz                 |
| FPU          | yes                          |
| TrustZone    | yes                          |
| Timers       | 16                           |
| UARTs        | 6 (3xUSART, 2xUART, 1xLPUART)|
| I2cs         | 4                            |
| SPIs         | 3                            |
| CAN          | 1                            |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l552ze.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00346336-stm32l552xx-and-stm32l562xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0264-stm32-cortexm33-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00615305-stm32l5-nucleo-144-board-mb1361-stmicroelectronics.pdf)|

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-L552ZE-Q board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD.
@note The upstream version of OpenOCD doesn't contain yet support for this board,
so the source code version from http://openocd.zylin.com/#/c/5510
must be built to be able to flash this board (adapt the configure command with
your preferred installation directory):

```
$ git clone https://git.code.sf.net/p/openocd/code openocd
$ cd openocd
$ git fetch http://openocd.zylin.com/openocd refs/changes/10/5510/5 && git checkout FETCH_HEAD
$ ./bootstrap
$ ./configure --prefix=<installation directory>
$ make -j
$ sudo make install
```

Once the patched OpenOCD is built and installed, you can flash the board simply
by typing

```
make BOARD=nucleo-l552ze-q flash
```

and debug via GDB by simply typing
```
make BOARD=nucleo-l552ze-q debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l552ze-q PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller LPUART1.

The default baud rate is 115 200.

If a physical connection to LPUART1 is needed, connect UART interface to pins
PG7 (LPUART1 TX) and PG8 (LPUART1 RX).

@note Accordingly to the [MCU Datasheet](https://www.st.com/resource/en/datasheet/stm32l552ze.pdf)
PG7 and PG8 pins are 5V tolerant (see table 21, page 108).

## Supported Toolchains

For using the ST Nucleo-L552ZE-Q board we recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
 */
