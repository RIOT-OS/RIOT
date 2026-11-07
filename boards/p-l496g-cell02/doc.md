@defgroup   boards_p-l496g-cell02 ST P-L496G-CELL02
@ingroup    boards
@brief      Support for the ST P-L496G-CELL02 board

## Overview

The ST [P-L496G-CELL02](https://www.st.com/en/evaluation-tools/p-l496g-cell02.html)
is an evaluation board supporting a ARM Cortex-M4 STM32L496AG microcontroller
with 320Kb of RAM and 1Mb of ROM Flash.

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the P-L496G-CELL2 is `p-l496g-cell02`.

### STDIO

STDIO is available via the ST-Link programmer.

Use the `term` target to open a terminal:

```shell
make BOARD=p-l496g-cell02 -C examples/basic/hello-world term
```
