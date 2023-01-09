/**
@defgroup    boards_nucleo-g474re STM32 Nucleo-G474RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G474RE

## Overview

The Nucleo-G474RE is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32G474RE microcontroller with 128KiB of RAM and 512KiB of Flash.

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-G474RE board includes an on-board ST-LINK V3 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-g474re flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-g474re debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-g474re PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


## Supported Toolchains

For using the ST Nucleo-G474RE board we recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
 */
