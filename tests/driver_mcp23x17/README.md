# Microchip MCP23x17 I2C I/O expanders test application

## Overview

This test appliation demonstrates the usage of the MCP23x17 driver interface
and can be used to test each MCP23x17 expander I/O pin with shell commands.

The application bases on the test application for GPIO peripheral drivers
which is under following copyright:

     Copyright (C) 2014,2017 Freie Universität Berlin
     @author Hauke Petersen <hauke.petersen@fu-berlin.de>

## Usage

To use the test application, compile it with one or more of the pseudomodules
`mcp23017` or `mcp23s17` to enable the driver for your expander modules.
Please check the default configuration parameters in
`$(RIOTBASE)/drivers/mcp23x17/include/mcp23x17_params.h` and adopt them
if necessary.

     USEMODULE=mcp23017 make -C tests/driver_mcp23x17 BOARD=...

@note When no pseudomodule is given, `mcp23017` is used by default.

The usage of the MCP23x17 low-active push-pull interrupt signal is
recommended to be able to use external interrupts with the expander I/O
pins. Add module `periph_gpio_irq` for this purpose and define the
MCU interrupt pin by parameter `MCP23X17_PARAM_INT_PIN`, e.g.

     CFLAGS="-DMCP23X17_PARAM_INT_PIN=\(GPIO\(0,6\)\)" \
     USEMODULE="mcp23017 periph_gpio_irq" make -C tests/driver_mcp23x17 BOARD=...
