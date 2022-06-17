/**
@defgroup    boards_nucleo-l412kb STM32 Nucleo-L412KB
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-L412KB

## Overview

The Nucleo-L412KB is a board from ST's Nucleo family supporting ARM-Cortex-M4
STM32L412KB microcontroller with 40KiB of RAM and 128KiB of Flash.

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l412kb PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

 */
