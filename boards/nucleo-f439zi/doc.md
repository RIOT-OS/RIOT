/**
@defgroup    boards_nucleo-f439zi STM32 Nucleo-F439ZI
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F439ZI

## Overview

The Nucleo-F439ZI is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F439ZI microcontroller with, 256KiB or RAM and 2MiB of Flash.

## Flashing the device

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f439zi PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

 */
