@defgroup    boards_nucleo-f413zh STM32 Nucleo-F413ZH
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F413ZH

## Overview

The Nucleo-F413ZH is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F413ZH microcontroller with 256KiB of RAM and 1MiB of Flash.

## Pinout

@image html pinouts/nucleo-f412zg-and-f413zh.svg "Pinout for the nucleo-f413zh (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 34)" width=50%

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f413zh PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).