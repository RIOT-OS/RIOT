/**
@defgroup    boards_nucleo-l053r8 STM32 Nucleo-L053R8
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L053R8

## Overview

The Nucleo-L053R8 is a board from ST's Nucleo family supporting a ARM
Cortex-M0+ STM32L053R8 microcontroller with 8KiB of RAM and 32KiB of Flash.

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:

```
make BOARD=nucleo-l053r8 PROGRAMMER=cpy2remed flash
```

@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

 */
