/**
@defgroup    boards_nucleo-f722ze STM32 Nucleo-F722ZE
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F722ZE

## Overview

The Nucleo-F722ZE is a board from ST's Nucleo family supporting ARM Cortex-M7
STM32F722ZE microcontroller with 256KiB of RAM and 512KiB of Flash.

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f722ze PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

 */
