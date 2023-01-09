/**
@defgroup    boards_nucleo-l433rc STM32 Nucleo-L433RC
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L433RC

## Overview

The Nucleo-L433RC is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32L433RC microcontroller with 64KiB of RAM and 256KiB of Flash.

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l433rc PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

 */
