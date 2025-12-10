---
title: Porting Boards
description: Guide on how to port new boards to RIOT-OS
---

At some point you might need to port a new `BOARD` to `RIOT`, either because
that specific development board is not yet supported or because you have a
custom `BOARD` for your project.

If you want to port a `BOARD` to `RIOT` you have two choices: doing it
inside of `RIOTBASE` or outside. In either case the file structure
is basically the same and moving from one to another is easy.

This guide details the generic structure you need to add a new `BOARD`
to `RIOT`, the different files as well as their functionality.

:::note
We assume here that your `CPU` and `CPU_MODEL` is already supported
in `RIOT` so no peripheral or cpu implementation is needed.
:::

## Porting Flowchart
![Porting flowchart](img/porting-boards.svg)

## General Structure

Like [applications](/advanced_tutorials/creating_applications/) or
[modules](/advanced_tutorials/creating_modules),
boards consist of a directory containing source files and
Makefiles. Usually a `BOARD` directory has the following structure,
although not all of the subdirectories or Makefiles have to be present for
a board implementation to work.

```
board-foo
├── dist
│   └── scripts
├── board.c
├── doc.md
├── include
│   ├── periph_conf.h
│   ├── board.h
│   └── gpio_params.h
├── Makefile
├── Makefile.dep
├── Makefile.features
└── Makefile.include
```

### Source Files

Header files in `board-foo/include` define physical mappings or
configurations. e.g:

- `periph_conf.h`: defines configurations and mappings for peripherals as well
  as clock configurations.
- `board.h`: holds board specific definitions or mappings, for example LEDs,
  buttons. It might as well override default drivers parameters (e.g.: assigning
  specific pin connections to a LCD screen, radio, etc.).
- `gpio_params.h`: if the board supports
  [SAUL](https://doc.riot-os.org/group__drivers__saul.html) then its
  [saul_gpio_params_t](https://doc.riot-os.org/structsaul__gpio__params__t.html)
  is defined here. (Analogously, an `adc_params.h` can
  contain a [saul_adc_params_t](https://doc.riot-os.org/structsaul__adc__params__t.html)
  and `pwm_params.h` a
  [saul_pwm_rgb_params_t](https://doc.riot-os.org/structsaul__pwm__rgb__params__t.html)
  and a [saul_pwm_dimmer_params_t](https://doc.riot-os.org/structsaul__pwm__dimmer__params__t.html)).
- other: other specific headers needed by one `BOARD`

:::note
Header files do not need to be defined in `include/`, but if defined
somewhere else then they must be added to the include path in
`Makefile.include`: `INCLUDES += -I<some>/<directory>/<path>`
:::

Board initialization functions are defined in `board.c`.
This file can define a `board_init()` function that is called at startup.
It is run before the scheduler is started, so it must not block (e.g. by
performing I2C operations).

```c
void board_init(void)
{
    /* initialize GPIO or others... */
    ...
}
```

### Makefiles

#### Makefile

A board's Makefile just needs to include `Makefile.base` in the RIOT
repository and define the `MODULE` as `board` (see
[modules](/advanced_tutorials/creating_modules) for more details)

```makefile
MODULE = board

include $(RIOTBASE)/Makefile.base
```

#### Makefile.dep

Dependencies on other `MODULES` or `FEATURES` can be defined here. This might
specify `MODULES` or dependencies that need to be pulled under specific
configurations. e.g.: if your board has a sx1276 lora chip:

```makefile
ifneq (,$(filter netdev_default,$(USEMODULE)))
  USEMODULE += sx1276
endif
```

:::note
`Makefile.dep` is processed only once so you have to take care of adding
the dependency block for your board *before* its dependencies pull in their own
dependencies.
:::

##### Default Configurations
There are two pseudomodules that are used to indicate that certain drivers of devices
present in the platform should be enabled. Each board (or CPU) has knowledge as
to which drivers should be enabled in each case.

The previous code snippet shows how a board which has a
[Semtech SX1272 and SX1276 radio driver](https://doc.riot-os.org/group__drivers__sx127x.html)
device, pulls in its driver when the default network interfaces are required.

When the pseudomodule `saul_default` is enabled, the board should pull in all
the drivers of the devices it has which provide a
[[S]ensor [A]ctuator [U]ber [L]ayer](https://doc.riot-os.org/group__drivers__saul.html)
interface. This is usually done as following:

```makefile
ifneq (,$(filter saul_default,$(USEMODULE)))
  USEMODULE += saul_gpio
  USEMODULE += apds9960
  USEMODULE += bmp280_i2c
  USEMODULE += lis3mdl
  USEMODULE += sht3x
endif
```

#### Makefile.features

This file defines all the features provided by the BOARD. These features
might also need to be supported by the `CPU`. Here, define the `CPU` and
`CPU_MODEL` (see [build system basics](/build-system/build_system_basics/)
for more details on these variables).

e.g.:

```makefile
CPU = foo
CPU_MODEL = foobar

# Put defined MCU peripherals here (in alphabetical order)
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_uart
```

#### Makefile.include

This file contains BSP or toolchain configurations for the `BOARD`. It
should at least define the configuration needed for flashing (i.e. specify a
default programmer) as well as the serial configuration (if one is available).
The default serial port configuration is provided by
`makefiles/tools/serial.inc.mk` and defines the following values for the serial
port (depending on the host OS):

```makefile
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))
```

So if the board is also using this, there's no need to redefine these variables
in the board configuration.

For example a board that is using a custom serial port (via an USB to serial
adapter) and that is flashed using OpenOCD by default would have the following
content in its `Makefile.include`:

```makefile
# Define the default port depending on the host OS
PORT_LINUX ?= /dev/ttyUSB0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbserial*)))

# this board uses OpenOCD
PROGRAMMER ?= openocd
```

### doc.md

Although not explicitly needed, if upstreamed and as a general good
practice, this file holds all `BOARD` documentation. This can include
datasheet reference, documentation on how to flash, etc.

The documentation must be under the proper doxygen group, you can compile the
documentation by calling `make doc` and then open the generated html file on
any browser.

```markdown
@defgroup    boards_foo FooBoard
@ingroup     boards
@brief       Support for the foo board
@author      FooName BarName <foo.bar@baz.com>

#### User Interface

  ...

#### Using UART

  ...

#### Flashing the device

  ...
```

Previously documentation was contained in `doc.txt` files with C-style comment
blocks. This style has been deprecated in favor of using `doc.md` files in
Markdown format, which eliminates formatting and interpretation issues.
Old style files will continually be replaced by the new format.

Up to version `0.9.2` the [riotgen](https://pypi.org/project/riotgen/) tool
will generate `doc.txt` files instead of `doc.md` files. You can upgrade it to
the latest version with

```bash
pip install --upgrade riotgen
```

## Timer Configuration

### Timer Width

The `ztimer` driver assumes a timer register bit-width of 32-bits by default.
If your microcontroller has a smaller  timer register (e.g. 16-bits), you have
to explicitly specify the maximum value the timer register can hold with the
`TIMER_0_MAX_VALUE` define.
This is the same value that is put in the `max` field of the `timer_config`
structure.
Typical values are `0x0000FFFFUL` for 16-bit wide timers, `0x00FFFFFFUL` for
24-bit wide timers and `0xFFFFFFFFUL` for 32-bit wide timers.

```c
static const timer_conf_t timer_config[] = {
    {
        [...]
        .max      = 0x0000ffff,
        [...]
    }
};

#define TIMER_0_MAX_VALUE   0x0000FFFFUL
```

:::caution
`ztimer` does not automatically check if the `max` field and the
`TIMER_0_MAX_VALUE` definition match!

For example: If you observe "erratic" blinking patterns in
`examples/basic/blinky`, make sure to check if the sizes match.
:::

### Overhead Calibration

When using the high level timer `ztimer` there is an overhead in calling the
[ztimer_sleep](https://doc.riot-os.org/group__sys__ztimer.html#gade98636e198f2d571c8acd861d29d360)
and [ztimer_set](https://doc.riot-os.org/group__sys__ztimer.html#ga8934a79a89e35d58673418a1e4a2e69c)
functions. This offset can be compensated for.
It can be measured by running `tests/sys/ztimer_overhead` on your board, i.e:

```bash
$ BOARD=my-new-board make -C tests/sys/ztimer_overhead flash term
```

This should give the following output:
```
main(): This is RIOT!
ZTIMER_USEC auto_adjust params:
    ZTIMER_USEC->adjust_set = xx
    ZTIMER_USEC->adjust_sleep = xx
ZTIMER_USEC auto_adjust params cleared
zitmer_overhead_set...
min=6 max=7 avg_diff=6
zitmer_overhead_sleep...
min=21 max=21 avg_diff=21
ZTIMER_USEC adjust params for my-new-board:
    CONFIG_ZTIMER_USEC_ADJUST_SET    6
    CONFIG_ZTIMER_USEC_ADJUST_SLEEP  21
```

The last two lines can be added as defines to the new board `board.h`:

```c
/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET     6
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   21
/** @} */
```

Alternatively, the pseudomodule
[ztimer_auto_adjust](https://doc.riot-os.org/group__pseudomodule__ztimer__auto__adjust.html)
can be used in an application to enable automatic timer offset compensation at board startup.
This however incurs overhead both in the text segment and at bootup time.

## Helper Tools

To help you start porting a board, the RIOT build system provides the
`generate-board` make target. It is a wrapper around the
[riotgen](https://pypi.org/project/riotgen/) command line tool that is helpful
when starting to port a board: all required files are generated with
copyright headers, doxygen groups, etc, so you can concentrate on the port.
The board source files are created in the `boards/<board name>` directory.

From the RIOT base directory, run:

```bash
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

## Common Board Directories

### Using Common Code

To avoid code duplication, common code across boards has been grouped in
`boards/common`. e.g. `BOARD`s based on the same cpu (`boards/common/nrf52`) or
`BOARD`s having the same layout `boards/common/nucleo64`.

In the case of source files this means some functions like `board_init` can be
already defined in the common code. Unless having specific configurations or
initialization you might not need a `board.c` or `board.h`. Another common use
case is common peripheral configurations, for example in the `cfg_timer_tim5.h`:

```c
/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */
```

#### New Style Common Code

The common board definitions of RIOT are currently being reworked to make the
usage of common code easier and less error prone. For example, if you want
to use the common code for the Adafruit nRF52 Bootloader that is used
by many of the nRF52 based boards from Adafruit, you simply have to add the
following line to the `Makefile.dep` of your board. Everything else
will be automatically included by the build system.

```makefile
USEMODULE += boards_common_adafruit-nrf52-bootloader
```

Not all common code is migrated to the new style yet, so if you are unsure
whether it is or not, you can check if the `boards/Makefile` already
includes a reference to the common code you want to use. If you are still
unsure, you can still use the Old Style Common Code or ask the
community.

#### Old Style Common Code

If you want to use common makefiles, include them at the end of the specific
`Makefile`, e.g. for a `Makefile.features`:

```makefile
CPU = foo
CPU_MODEL = foobar
# Put defined MCU peripherals here (in alphabetical order)
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_uart
include $(RIOTBOARD)/common/foo_common/Makefile.features
```

If the common code includes source files, it might be necessary
to explicitly include the directory in your `Makefile` so the Make system
finds all the necessary files:

```makefile
MODULE = board

DIRS += $(RIOTBOARD)/common/myCommonFolder

include $(RIOTBASE)/Makefile.base
```

If possible, you should use the New Style Common Code though.

### Moving Common Code to a Dedicated Folder

If you port a board that is very similar to an already existing board, it might
make sense to move the shared code to a common directory located in
`boards/common` to use it as described in the previous section.

The directory structure of a common folder is very similar to the board
folder structure and not all files and folders have to be present except for
the main `Makefile`.

```
RIOT
└── boards
    └── common
        └── adafruit-nrf52-bootloader
            ├── board.c
            ├── doc.md
            ├── include
            │   ├── periph_conf.h
            │   ├── board.h
            │   └── gpio_params.h
            ├── Makefile
            ├── Makefile.dep
            ├── Makefile.features
            └── Makefile.include
```

The main `Makefile` defines the module name for the common board module and
should follow the general naming scheme of `boards_common_awesome-common-stuff`.

```makefile
MODULE = boards_common_adafruit-nrf52-bootloader

include $(RIOTBASE)/Makefile.base
```

The `Makefile.dep`, `Makefile.features` and `Makefile.include` are optional
and work the same way as their normal board pendants.

To inform the build system about the common folders and Makefiles, the
`boards/Makefile`, `boards/Makefile.dep`, `boards/Makefile.features` and
`boards/Makefile.include` files have to be modified.

The `boards/Makefile` contains the directory entries for the common files.
The entries should check if the common module is used and conditionally add
the directory to the `DIRS` variable.
Please note that the entries should be sorted alphabetically.
```makefile
# SORT THIS ALPHABETICALLY BY COMMON BOARD NAME!
...
ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/adafruit-nrf52-bootloader
endif
...
```

The `boards/Makefile.dep`, `boards/Makefile.features` and
`boards/Makefile.include` just include their common counterparts. As an
example, an entry of the `boards/Makefile.dep` is shown:
```makefile
# SORT THIS ALPHABETICALLY BY COMMON BOARD NAME!
...
ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  include $(RIOTBOARD)/common/adafruit-nrf52-bootloader/Makefile.dep
endif
...
```

You only have to add entries to the `board/Makefile`s if your common code
actually has the regarding Makefile-type.

## Boards Outside of RIOTBASE

All `BOARD`s in RIOT reside in `RIOTBOARD` (`RIOTBOARD` being a make variable
set to `$(RIOTBOARD)/boards`).

If one wants to use a `BOARD` outside of `RIOTBOARD`, the way to go is setting
the `EXTERNAL_BOARD_DIRS` variable to the path to the directory containing your
external boards, e.g.: `EXTERNAL_BOARD_DIRS=/home/external-boards/` (this would
commonly be done in your application `Makefile` or your environment). You can
specify multiple directories separated by spaces.

```
/home
├── RIOT
│   └── ...
└── external-boards
    └── board-foo
        ├── dist
        │   └── scripts
        ├── board.c
        ├── doc.md
        ├── include
        │   ├── periph_conf.h
        │   ├── board.h
        │   └── gpio_params.h
        ├── Makefile
        ├── Makefile.dep
        ├── Makefile.features
        └── Makefile.include
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

```makefile
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
  [Using Common Code](#using-common-code)). e.g:
  `include $(RIOTBOARD)/foo-parent/Makefile.*include*`

An example can be found in
[`tests/build_system/external_board_native`](https://github.com/RIOT-OS/RIOT/tree/master/tests/build_system/external_board_native).

## Board Names and Aliases

New boards should be named according to
[RDM0003](https://github.com/RIOT-OS/RIOT/blob/master/doc/memos/rdm0003.md).
Historically, some board names have not followed this structure.
For backwards compatibility, RIOT supports board aliases that can be used
in place of the actual board name in the environment or Make variable `BOARD`.

A list of all existing board aliases can be found in
[`makefiles/board_alias.inc.mk`](https://github.com/RIOT-OS/RIOT/blob/master/makefiles/board_alias.inc.mk).
[`BOARD=native`](https://doc.riot-os.org/group__boards__common__native.html)
is a special alias in that it
resolves to either [`native32`](https://doc.riot-os.org/group__boards__native32.html)
or [`native64`](https://doc.riot-os.org/group__boards__native64.html)
depending on the host architecture.

## Tools

Some scripts and tools available to ease `BOARD` porting and testing:

  - Run `dist/tools/insufficient_memory/update_insufficient_memory_board.sh <board>`
    if your board has little memory. This updates the `Makefile.ci` lists to
    exclude the `BOARD` from automated compile-tests of applications that do
    not fit on the `BOARD`s `CPU`.
    Please keep in mind that this will take quite a while depending on your
    system!

  - Run `dist/tools/compile_and_test_for_board/compile_and_test_for_board.py . <board> --with-test-only`
    to run all automated tests on the new board.

## Further Reference

- [In her blog](https://blog.martine-lenders.eu/riot-board-en.html), Martine Lenders documented her approach of
  porting the [Adafruit Feather nRF52840 Express](https://doc.riot-os.org/group__boards__adafruit-feather-nrf52840-express.html)
  in February 2020.
- [Over at HackMD](https://hackmd.io/njFHwQ33SNS3sQKAkLkNtQ), Akshai M documented his approach of
  porting the [Silicon Labs SLSTK3400A starter kit](https://doc.riot-os.org/group__boards__slstk3400a.html) in July 2020.
