Porting boards                                                 {#porting-boards}
================

At some point you might need to port a new `BOARD` to `RIOT`, either because
that specific development board is not yet supported or because you have a
custom `BOARD` for your project.

If you want to port a `BOARD` to `RIOT` you have two choices: doing it
inside of `RIOTBASE` or outside. In either case the file structure
is basically the same and moving from one to another is easy.

This guide details the generic structure you need to add a new `BOARD`
to `RIOT`, the different files as well as their functionality.

@note We assume here that your `CPU` and `CPU_MODEL` is already supported
in `RIOT` so no peripheral or cpu implementation is needed.

# Porting flowchart                                         {#porting-flowchart}
@dotfile porting-boards.dot

# General structure                                         {#general-structure}

Like @ref creating-an-application "applications" or @ref creating-modules
"modules", boards consist on a directory containing source files and
makefiles. Usually a `BOARD` directory has the following structure

```
  board-foo/
  |----dist/
      |----scripts
  |----board.c
  |----doc.txt
  |----include/
      |----periph_conf.h
      |----board.h
      |----gpio_params.h
  |----Makefile
  |----Makefile.dep
  |----Makefile.features
  |----Makefile.include
```

## Source files                                            {#board-source-files}

Header files in `board-foo/include` define physical mappings or
configurations. e.g:

- `periph_conf.h`: defines configurations and mappings for peripherals as well
  as clock configurations.
- `board.h`: holds board specific definitions or mappings, for example LEDs,
  buttons. It might as well override default drivers parameters (e.g.: assigning
  specific pin connections to a LCD screen, radio, etc.). Some boards might also
  define optimized `XTIMER_%` values (e.g. @ref XTIMER_BACKOFF).
- `gpio_params.h`: if the board supports @ref drivers_saul "SAUL" then its
  @ref saul_gpio_params_t is defined here. (Analogously, a `adc_params.h` can
  contain a @ref saul_adc_params_t, and `pwm_params.h` a @ref
  saul_pwm_rgb_params_t and a @ref saul_pwm_dimmer_params_t).
- other: other specific headers needed by one `BOARD`

@note Header files do not need to be defined in `include/`, but if defined
somewhere else then they must be added to the include path. In
`Makefile.include`: `INCLUDES += -I<some>/<directory>/<path>`

Board initialization functions are defined in `board.c`. This file must at
least define a `board_init()` function that is called at startup.
It is run before the scheduler is started, so it must not block (e.g. by
performing I2C operations).

```c
void board_init(void)
{
    /* initialize GPIO or others... */
    ...
}
```

## Makefiles

### Makefile                                                         {#Makefile}

A board's Makefile just needs to include `Makefile.base` in the RIOT
repository and define the `MODULE` as `board` (see @ref creating-modules
"modules" for more details)

```mk
MODULE = board

include $(RIOTBASE)/Makefile.base
```

### Makefile.dep                                                 {#makefile-dep}

Dependencies on other `MODULES` or `FEATURES` can be defined here. This might
specify `MODULES` or dependencies that need to be pulled under specific
configurations. e.g.: if your board has a sx1276 lora chip:

```mk
ifneq (,$(filter netdev_default,$(USEMODULE)))
  USEMODULE += sx1276
endif
```

@note `Makefile.dep` is processed only once so you have to take care of adding
the dependency block for your board *before* its dependencies pull in their own
dependencies.

#### Default configurations
As explained in @ref default-configurations "Default Configurations", there are
two pseudomodules that are used to indicate that certain drivers of devices
present in the platform should be enabled. Each board (or CPU) has knowledge as
to which drivers should be enabled in each case.

The previous code snippet shows how a board which has a @ref drivers_sx127x
device, pulls in its driver when the default network interfaces are required.

When the pseudomodule `saul_default` is enabled, the board should pull in all
the drivers of the devices it has which provide a @ref drivers_saul interface. This is
usually done as following:

```mk
ifneq (,$(filter saul_default,$(USEMODULE)))
  USEMODULE += saul_gpio
  USEMODULE += apds9960
  USEMODULE += bmp280_i2c
  USEMODULE += lis3mdl
  USEMODULE += sht3x
endif
```

### Makefile.features                                       {#makefile-features}

This file defines all the features provided by the BOARD. These features
might also need to be supported by the `CPU`. Here, define the `CPU` and
`CPU_MODEL` (see @ref build-system-basics "build system basics" for more details
 on these variables).

e.g.:

```mk
CPU = foo
CPU_MODEL = foobar

# Put defined MCU peripherals here (in alphabetical order)
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_uart
```

### Makefile.include                                         {#makefile-include}

This file contains BSP or toolchain configurations for the `BOARD`. It
should at least define the configuration needed for flashing (i.e. specify a
default programmer) as well as the serial configuration (if one is available).
The default serial port configuration is provided by
`makefiles/tools/serial.inc.mk` and define the following values for the serial
port (depends on the host OS):

```
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))
```

So if the board is also using this, there's no need to redefine these variables
in the board configuration.

For example a board that is using a custom serial port (via an USB to serial
adapter) and that is flashed using openocd by default would have the following
content in its `Makefile.include`:

```mk
# Define the default port depending on the host OS
PORT_LINUX ?= /dev/ttyUSB0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbserial*)))

# this board uses openocd
PROGRAMMER ?= openocd
```

## doc.txt                                                          {#board-doc}

Although not explicitly needed, if upstreamed and as a general good
practice, this file holds all `BOARD` documentation. This can include
datasheet reference, documentation on how to flash, etc.

The documentation must be under the proper doxygen group, you can compile the
documentation by calling `make doc` and then open the generated html file on
any browser.

@code
/**
@defgroup    boards_foo FooBoard
@ingroup     boards
@brief       Support for the foo board
@author      FooName BarName <foo.bar@baz.com>

### User Interface

  ....

### Using UART

  ...

### Flashing the device

  ...

*/
@endcode

# Helper tools

To help you start porting a board, the RIOT build system provides the
`generate-board` make target. It is a wrapper around the
[riotgen](https://pypi.org/project/riotgen/) command line tool that is helpful
when starting to port a board: all required files are generated with
copyright headers, doxygen groups, etc, so you can concentrate on the port.
The board source files are created in the `boards/<board name>` directory.

**Usage:**

From the RIOT base directory, run:
```
make generate-board
```
Then answer a few questions about the driver:
- Board name: Enter a name for your board. It will be used as the name
  of the board directory under `boards`.
- Board displayed name: Enter the name of the board, as displayed in the
  Doxygen documentation.
- CPU name: Enter the name of the CPU embedded on the board.
- CPU model name: Enter the precise model name of the CPU.
- Features provided: CPU features provided (and configured) for this board.

Other global information (author name, email, organization) should be retrieved
automatically from your git configuration.

# Using Common code                                         {#common-board-code}

To avoid code duplication, common code across boards has been grouped in
`boards/common`. e.g. `BOARD`s based on the same cpu (`boards/common/nrf52`) or
`BOARD`s having the same layout `boards/common/nucleo64`.

In the case of source files this means some functions like `board_init` can be
already defined in the common code. Unless having specific configurations or
initialization you might not need a `board.c` or `board.h`. Another common use
case is common peripheral configurations:

@code
-\#include "cfg_timer_tim5.h"
+/**
+ * @name   Timer configuration
+ * @{
+ */
+static const timer_conf_t timer_config[] = {
+    {
+        .dev      = TIM5,
+        .max      = 0xffffffff,
+        .rcc_mask = RCC_APB1ENR_TIM5EN,
+        .bus      = APB1,
+        .irqn     = TIM5_IRQn
+    }
+};
+
+#define TIMER_0_ISR         isr_tim5
+
+#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
+/** @} */
@endcode

If you want to use common makefiles, include them at the end of the specific
`Makefile`, e.g. for a `Makefile.features`:

```mk
CPU = foo
CPU_MODEL = foobar

# Put defined MCU peripherals here (in alphabetical order)
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_uart

include $(RIOTBOARD)/common/foo_common/Makefile.features
```

# Boards outside of RIOTBASE                       {#boards-outside-of-riotbase}

All `BOARD`s in RIOT reside in `RIOTBOARD` (`RIOTBOARD` being a make variable
set to `$(RIOTBOARD)/boards`).

If one wants to use a `BOARD` outside of `RIOTBOARD`, the way to go is setting
the `EXTERNAL_BOARD_DIRS` variable to the path to the directory containing your
external boards, e.g.: `EXTERNAL_BOARD_DIRS=/home/external-boards/` (this would
commonly be done in your application `Makefile` or your environment). You can
specify multiple directories separated by spaces.

```
/home/
|----RIOT/
    |---- ...
|----external-boards/
    |----board-foo/
        |----dist/
            |----scripts
        |----board.c
        |----doc.txt
        |----include/
            |----periph_conf.h
            |----board.h
            |----gpio_params.h
        |----Makefile
        |----Makefile.dep
        |----Makefile.features
        |----Makefile.include
```

If the external `BOARD` is very similar to a `BOARD` already present in
`RIOTBOARD`, the external `BOARD` (`board-foo`) can inherit from that
parent `BOARD` (e.g: `foo-parent`).

In this case some special considerations must be taken with the makefiles:

- `Makefile`
  - `MODULE` cannot be `board`: `foo-parent` will already define
    `MODULE = board`, so use any other name, lets say `MODULE = board-foo`.
  - Include the location of the parent `BOARD` to inherit from (if there is
    one):

```mk
      DIRS += $(RIOTBOARD)/foo-parent
```

- `Makefile.include`
  - duplicate the include done by `$(RIOTBASE)/Makefile.include` to also
    include the parent board header. e.g: if inheriting from `foo-parent`
    ``INCLUDES += $(addprefix -I,$(wildcard $(RIOTBOARD)/foo-parent/include))`

- `Makefile.dep`: `board` is added by default to `USEMODULE` but since
  `board-foo` is used for this `BOARD`, it must be explicitly included by adding
  `USEMODULE += board-foo`.

- Then simply include in each `Makefile.*` the corresponding parent `BOARD`
  `Makefile.*`, just as it is done for common `BOARD` code (as explained in
  @ref common-board-code). e.g:
  `include $(RIOTBOARD)/foo-parent/Makefile.*include*`

An example can be found in
[`tests/external_board_native`](https://github.com/RIOT-OS/RIOT/tree/master/tests/external_board_native)

# Tools                                                          {#boards-tools}

Some scripts and tools available to ease `BOARD` porting and testing:

  - Run `dist/tools/insufficient_memory/add_insufficient_memory_board.sh <board>`
    if your board has little memory. This updates the `Makefile.ci` lists to
    exclude the `BOARD` from automated compile-tests of applications that do
    not fit on the `BOARD`s `CPU`.

  - Run `dist/tools/compile_and_test_for_board/compile_and_test_for_board.py . <board> --with-test-only`
    to run all automated tests on the new board.

# Further reference                                         {#further-reference}

- [In her blog][martines-blog], Martine Lenders documented her approach of
  porting the @ref boards_feather-nrf52840 in February 2020.
- [Over at HackMD][hackmd-slstk3400a], Akshai M documented his approach of
  porting the @ref boards_slstk3400a in July 2020.

[martines-blog]: https://blog.martine-lenders.eu/riot-board-en.html
[hackmd-slstk3400a]: https://hackmd.io/njFHwQ33SNS3sQKAkLkNtQ
