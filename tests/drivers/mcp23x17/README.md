# Microchip MCP23x17 I2C I/O expanders test application

## Overview

This test application demonstrates the usage of the MCP23x17 driver
interface and can be used to test each MCP23x17 expander I/O pin with shell
commands.

The application bases on the test application for GPIO peripheral drivers
which is under following copyright:

Copyright (C) 2014,2017 Freie Universität Berlin
Author Hauke Petersen <hauke.petersen@fu-berlin.de>

## Compilation

Used MCP23x17 I/O expander or interface variants have to be defined by
the variable `DRIVER` using the corresponding pseudomodules `mcp23017`,
`mcp23s17`, `mcp23x17_i2c` and/or `mcp23x17_spi`, for example:

```
DRIVER='mcp23x17_i2c mcp23x17_spi' BOARD=... make -C tests/drivers/mcp23x17 flash term
```

**Please note:** If `DRIVER` is not defined, `mcp23x17_i2c` is used by default.

The default hardware configuration parameters are defined in
`$(RIOTBASE)/drivers/mcp23x17/include/mcp23x17_params.h`. They can
be overridden either by placing a `mcp23x17_params.h` file with changed
configuration parameters in the directory of the test application or by
defining them in the `CFLAGS` variable in the make command, for example:

```
CFLAGS="-DMCP23X17_PARAM_RESET_PIN=GPIO_PIN\(0,7\)" \
USEMODULE=mcp23x17_reset \
BOARD=... make -C tests/drivers/mcp23x17 flash term
```

To use external interrupts for the MCP23x17 I/O expander pins, the MCP23x17
low-active push/pull interrupt signal has to be enabled. For this purpose
add the module `mcp23x17_irq` and define the GPIO pin to which the
combined `INTA`/`INTB` interrupt signal is connected, e.g. by overriding the
default parameter `MCP23X17_PARAM_I2C_INT` for the default I2C device or
`MCP23X17_PARAM_SPI_INT` for the default SPI device.

```
CFLAGS="-DMCP23X17_PARAM_I2C_INT=GPIO_PIN\(0,6\)" \
USEMODULE=mcp23x17_irq \
BOARD=... make -C tests/drivers/mcp23x17 flash term
```

Using module `mcp23x17_irq` uses by default an event thread with medium
priority. An event thread with high priority can be used by enabling
the module `mcp23x17_irq_highest`.

**Please note:** Since interrupts are handled in the context of a separate event thread,
enabling interrupts requires more RAM.

## Usage

The test allows to use commands as known from GPIO test application for
MCP23x17 I/O expanders:

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
enable_int           enable gpio interrupt
disable_int          disable gpio interrupt
read                 read pin status
set                  set pin to HIGH
clear                set pin to LOW
toggle               toggle pin
bench                run a set of predefined benchmarks
```

The number of the first MCP23x17 I/O expander port used by the test
application is defined by the macro `MCP23X17_PORT_0`, which is 16 by
default. This value can be overridden during compilation, for example:

```
CFLAGS="-DMCP23X17_PORT_0=8" \
BOARD=... make -C tests/drivers/mcp23x17 flash term
```

Using the port number defined by `MCP23X17_PORT_0` and following port
numbers, you can apply the command to the MCP23x17 expander ports. For
example, the following command initializes I/O pin 7 of the first MCP23x17
expander:

```
init_out 16 7
```

Commands with port numbers less than `MCP23X17_PORT_0` refer to GPIO
peripheral ports. Thus, both the I/O pins of the MCP23x17 expanders as well
as the GPIO peripheral pins of the MCU can be addressed by all commands.

Thus it is possible to connect a GPIO pin of the MCU with MCP23x17
I/O expander pins for testing.
