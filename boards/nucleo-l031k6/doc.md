/**
@defgroup    boards_nucleo-l031k6 STM32 Nucleo-L031K6
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-L031K6

## Overview

The Nucleo-L031K6 is a board from ST's Nucleo family supporting ARM Cortex-M0
STM32L031K6T6 microcontroller with 8KiB or RAM and 32KiB of Flash.


## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l031k6 PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


*/
