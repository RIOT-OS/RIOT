/**
 @defgroup    boards_nucleo-f412zg STM32 Nucleo-F412ZG
 @ingroup     boards_common_nucleo144
 @brief       Support for the STM32 Nucleo-F412ZG

## Overview

The Nucleo-F412ZG is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F412ZG microcontroller with 256KiB of RAM and 1MiB of Flash.

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f412zg PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw
-link007.html).

*/
