# Texas Instruments PCF857X I2C I/O expanders test application

## Overview

This test appliation demonstrates the usage of the PCF857X driver interface
and can be used to test each PCF857X expander I/O pin with shell commands.

## Usage

To use the test application, compile it with one or more of the pseudomodules
`pcf8574`, `pcf8574a` or `pcf8575` to enable the driver for your
expander modules. Please check the default configuration parameters in
`$(RIOTBASE)/drivers/pcf857x/include/pcf857x_params.h` and adopt them
if necessary. Alternatively, a modified version of this file could be
placed in the directory of this test application to override it.

     USEMODULE=pcf8575 make -C tests/driver_pcf857x BOARD=...

**Please note:** When no pseudomodule is given, `pcf8575` is used by default.

To use external interrupts with the expander I/O pins, the PCF857X
low-active open-drain interrupt signal has to be enabled. Add module
`pcf857x_irq` for this purpose and define the MCU interrupt pin by
parameter `PCF857X_PARAM_INT_PIN`, e.g.

     CFLAGS="-DPCF857X_PARAM_INT_PIN=\(GPIO_PIN\(0,6\)\)" \
     USEMODULE="pcf8575 pcf857x_irq" make -C tests/driver_pcf857x BOARD=...

**Please note:** Since interrupts are handled in the context of a separate
event thread, enabling interrupts requires more RAM.
