@defgroup    boards_infineon-psoc-62 Infineon PSoC 6 AI Kit
@ingroup     boards
@brief       Support for the Infineon PSoC 6 AI Kit

## Overview

This board uses an Infineon PSoC 6 device with a Cortex-M0+ boot core and a
Cortex-M4 application core.

## Flashing the board

The board is flashed via OpenOCD using the board-specific configuration in
[dist/openocd.cfg](dist/openocd.cfg).

This target requires the Infineon / ModusToolbox OpenOCD build. Stock OpenOCD
packages from Linux distributions may fail because they do not include the
PSoC-specific target and flash driver support required by this board.

If the ModusToolbox OpenOCD binary is in your `PATH`, flashing works as usual:

```bash
make BOARD=infineon-psoc-62 -C examples/basic/hello-world flash
```

If it is not in your `PATH`, set `MTB_OPENOCD_BASE` to the ModusToolbox OpenOCD
directory:

```bash
make BOARD=infineon-psoc-62 \
     MTB_OPENOCD_BASE=/opt/Tools/ModusToolboxProgtools-1.6/openocd \
     -C examples/basic/hello-world flash
```

This sets:

```text
OPENOCD=<MTB_OPENOCD_BASE>/bin/openocd -s <MTB_OPENOCD_BASE>/scripts
```

which makes the required Infineon target scripts available to OpenOCD.
