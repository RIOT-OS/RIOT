@defgroup    boards_stm32f723e-disco STM32F723E-DISCO board
@ingroup     boards
@brief       Support for the STM32F723E-DISCO board

## Overview

The ST [STM32F723E-DISCO](https://www.st.com/en/evaluation-tools/32f723ediscovery.html)
is an evaluation board supporting a ARM Cortex-M7 STM32F723IE microcontroller
with 256KB of RAM and 523KB of ROM Flash.

## Flashing the device

The STM32F723E-DISCO board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD.

To flash this board, just use the following command:

```
make BOARD=stm32f723e-disco flash -C examples/basic/hello-world
```

### STDIO

STDIO is available via the ST-Link programmer.

Use the `term` target to open a terminal:

    make BOARD=stm32f723e-disco -C examples/basic/hello-world term

### USB OTG Peripheral Device Driver

By default, the USB OTG FS port is used. To use the USB OTG HS port with the
internal UTMI+ HS PHY, enable the module `periph_usbdev_hs_utmi`:

    make BOARD=stm32f723e-disco USEMODULE=periph_usbdev_hs_utmi -C examples/advanced/usbus_minimal
