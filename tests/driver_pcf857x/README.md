# Texas Instruments PCF857X I2C I/O expanders test application

## Overview

This test appliation demonstrates the usage of the PCF857X driver interface
and can be used to test each PCF857X expander I/O pin with shell commands.

## Compilation

To use the test application, compile it with one or more of the pseudomodules
`pcf8574`, `pcf8574a` or `pcf8575` to enable the driver for your
expander modules. Please check the default configuration parameters in
`$(RIOTBASE)/drivers/pcf857x/include/pcf857x_params.h` and modify them
if necessary. Alternatively, a modified version of this file could be
placed in the directory of this test application to override it.
```
USEMODULE=pcf8575 make -C tests/driver_pcf857x BOARD=...
```
**Please note:** When no pseudomodule is given, `pcf8575` is used by default.

To use external interrupts with the expander I/O pins, the PCF857X
low-active open-drain interrupt signal has to be enabled. Add module
`pcf857x_irq` for this purpose and define the MCU interrupt pin by
parameter `PCF857X_PARAM_INT_PIN`, e.g.
```
CFLAGS="-DPCF857X_PARAM_INT_PIN=\(GPIO_PIN\(0,6\)\)" \
USEMODULE="pcf8575 pcf857x_irq" make -C tests/driver_pcf857x BOARD=...
```
**Please note:** Since interrupts are handled in the context of a separate
event thread, enabling interrupts requires more RAM.

## Usage

The test allows to use commands as known from GPIO test application for
PCF857X expanders:
```
> help
Command              Description
---------------------------------------
init_out             init as output (push-pull mode)
init_in              init as input w/o pull resistor
init_in_pu           init as input with pull-up
init_od              init as output (open-drain without pull resistor)
init_od_pu           init as output (open-drain with pull-up)
init_int             init as external INT w/o pull resistor
enable_int           enable or disable gpio interrupt
read                 read pin status
set                  set pin to HIGH
clear                set pin to LOW
toggle               toggle pin
bench                run a set of predefined benchmarks

```
The number of the first PCF857X expander port used by the test application
is defined by the macro `PCF857X_PORT_0`, which is 16 by default. This value
can be overridden during compilation, e.g.:
```
CFLAGS="-DPCF857X_PORT_0=8" \
USEMODULE=pcf8575 make -C tests/driver_pcf857x BOARD=...
```
Using the port number defined by `PCF857X_PORT_0` and the following port
numbers, you can apply the command to the PCF857X expander ports. For
example, the following command initializes I/O pin 7 of the first PCF857X
expander:
```
init_out 16 7
```
Commands with port numbers less than `PCF857X_PORT_0` refer to GPIO
peripheral ports. Thus, both the I/O pins of the PCF857X expanders as well
as the GPIO peripheral pins of the MCU can be addressed by all commands.
