---
title: Build System Basics
description: This page describes the basic concepts of the RIOT build system.
---

## FEATURES

### What is a FEATURE?

A `FEATURE` is a mean of specifying valid/invalid dependencies and configurations.

Whenever a `FEATURE` is used there should be at some level a hardware requirement,
whether this is a _radio_, a _bus_ of a specific core architecture.

This is not a hard line, in some cases the line can be harder to establish than
others. There are complicated cases like `netif` since a network interface could
be fully implemented in software as a loop-back.

It's also important to note that a `FEATURE` does not mean there is a `MODULE`
with the same name. There could be many implementations for the same `FEATURE`.
The fact that many `FEATURES` translate directly into a `MODULE` is only by
convenience.

e.g.
```makefile
# all periph features correspond to a periph submodule
USEMODULE += $(filter periph_%,$(FEATURES_USED))
```

### Providing a FEATURE

For a `FEATURE` to be provided by a `board` it must meet 2 criteria, and for
`periph_%` and other _hw_ (hardware) related `FEATURES` it must follow a 3rd criteria.

- Needs the "hardware" or BSP support (toolchain, build system, flasher, etc.)
  - e.g.: `stm32l152re` has an SPI peripheral
    `riotboot` needs to be able to link and flash at an offset
- Needs support in RIOT, an implementation of an api to interact with the _hw_
  - e.g.: `cpu/stm32_common/periph/spi.c` is implemented for `stm32l1`
    `riotboot` needs an implementation of `cpu_jump_to_image`
- Wiring between the _cpu/soc_(system on a chip) a _bus_ and other _cpu_/_hw_ components.
  - e.g.: `nucleo-l152re/include/periph_conf.h` specified wiring between `PORT_Ax`
    and `SPI1`

### All the FEATURES\_%

- `FEATURES_PROVIDED` are available hardware (including BSP) features
  (e.g.:`periph_hwrng`, `periph_uart`) or characteristics (e.g:`arch_32bits`) of
  a board.

- `FEATURES_CONFLICT` are a series of `FEATURES` that can't be used at the same
  time for a particular `BOARD`.

- `FEATURES_REQUIRED` are `FEATURES` that are needed by a `MODULE` or `APPLICATION`
  to work.

- `FEATURES_OPTIONAL` are "nice to have" `FEATURES`, not needed but useful. If
  available they are always included.

- `FEATURES_REQUIRED_ANY` are `FEATURES` of which (at least) one of
  is needed by a `MODULE` or `APPLICATION`. Alternatives are separated by
  a pipe (`|`) in order of preference, e.g.:
  `FEATURES_REQUIRED_ANY += arch_avr8|arch_native` if both are provide then
  `arch_avr8` will be used.

- `FEATURES_BLACKLIST` are `FEATURES` that can't be used by a `MODULE` or `APPLICATION`.
  They are usually used for _hw_ characteristics like `arch_` to easily resolve
  unsupported configurations for a group.

- `FEATURES_USED` are the final list of `FEATURES` that will be used by an `APPLICATION`

### Where to define FEATURES\_%

- `FEATURES_PROVIDED`, `FEATURES_CONFLICT` and `FEATURES_CONFLICT_MSG` are
  defined in `Makefile.features`

- `FEATURES_REQUIRED`, `FEATURES_OPTIONAL`, `FEATURES_REQUIRED_ANY`,
  and `FEATURES_BLACKLIST` are defined by the application `Makefile`
  (`examples/%/Makefile`, `tests/%/Makefile`, etc.) or in `Makefile.dep`

## CPU/CPU_MODEL

`CPU` and `CPU_MODEL` refer to the _soc_ or _mcu_ (microcontroller)
present in a `BOARD`. The variables `CPU`, `CPU_FAM`, etc. are just arbitrary groupings
to avoid code duplication. How this grouping is done depends on every implementation
and the way each manufacturer groups there products.

These variables allows declaring the `FEATURES` that the _mcu/soc_ provides as well
as resolving dependencies.

`FEATURES` provided by a `CPU/CPU_MODEL` should not depend on the wiring of a
specific `BOARD` but be intrinsic to the _soc/mcu_.

A `CPU/CPU_MODEL` might support `FEATURES` that will depend on the `BOARD` wiring,
e.g.: bus (`uart`, `spi`) mappings. In this cases the `FEATURE` should be provided
by the `BOARD.`

## BOARD

In RIOTs build-system, a `BOARD` is a grouping of:

- _soc/mcu_ (`CPU/CPU_MODEL`)
  - e.g.: `b-l072z-lrwan1` `stm32l072cz`
- _sensor/actuators_ (buttons and leds included) (`drivers`)
  - e.g.: `b-l072z-lrwan1` leds and buttons
- _radios_, _ethernet_, etc. devices (`drivers`)
  - e.g.: `b-l072z-lrwan1` `sx1276`
- _programming/debugging_ tools
  - e.g.: `b-l072z-lrwan1` `stlink`
- configuration mapping cpu support capabilities to availability
  - e.g.: `b-l072z-lrwan1` `periph_conf.h`, `gpio_params`

A `board` can have all the required `FEATURES` to interact with a radio or
sensor/actuator, but it doesn't necessarily provide that `FEATURE`.

e.g.: - `samr21-xpro` provides a `at86rf233` radio as well as the necessary
`periph_*` features. - `nucleo-*` provide all `periph_*` features to use `sx1272`, and
even a default configuration for the `SX1272MB2xA` shield, but not
doesn't provide the radio.

If a `board` in `$(RIOTBASE)/boards` is connected to a radio shield, sensors,
actuators, etc. then it is a different `board` than the one provided by default.
Whenever you need to have a device mapping (in linux-arm, it would require
a different device tree), then it is a different board and would need a
different `board/periph_conf`.

A `nucleo-*` with a `SX1272MB2xA` is a different board in RIOT sense.

_note_: if `devicetree` is implemented this concept will change.

# Variables declaration guidelines

This page contains basic guidelines about `make` variable declaration, it
summarizes some of the pros and cons as well as specifies good and bad patterns
in our build system. You might want to refer to `gnu make` documentation
regarding these subjects.

## Avoid Unnecessary Export

```makefile
export OUTPUT = $(shell some-command)
```

Exporting a variable means it will be evaluated on every `target` call, which
slows down the build system. Always avoid exporting a variable if unneeded.

If an export is actually needed by a `sub-make` then export the variable only for
the needed targets using `target-export-variables` (more in
`makefiles/utils/variables.mk`).

Exported variables ("global variable") are hard to remove, specially when badly
documented. If no one knows why it's there and no one knows where it can be used
then no one knows if it's safe to remove since it's present for every target.
This is why global variables need clear documentation.

[gnumake doc](https://www.gnu.org/software/make/manual/html_node/Variables_002fRecursion.html)

## Use Memoized for Variables Referencing a Function or Command

### Recursively Expanded Variable

```makefile
OUTPUT = $(shell some-command $(ANOTHER_VARIABLE))
```

- When using `=` the value of the variable is only declared, but not set,
  therefore the variable will only be evaluated when expanded (used) somewhere
  in the makefile. If `$(OUTPUT)` is never expanded, `some-command`
  is never executed and `ANOTHER_VARIABLE` not expanded.

- All variables or functions referenced by the declared variable will will be
  evaluated every time the variable is expanded.
  In the example `some-command` is executed every time `OUTPUT` is expanded, same for
  `ANOTHER_VARIABLE`. If `some-command` is slow this introduced unneeded overhead.

- If the variable expansion doesn't involve evaluating a function the overhead
  is none.

### Simply Expanded Variable

```makefile
OUTPUT := $(shell some-command $(ANOTHER_VARIABLE))
```

- When using `:=` the value is only expanded once, expanding any reference to
  other variables or functions. If `OUTPUT` is always used at least once and
  evaluates a costly function (`some command`) then use `:=`.

- When using `:=` the variable will be evaluated even if not needed, which
  introduces unnecessary delay, in particular `some command` or functions
  evaluated by `ANOTHER_VARIABLE` are slow.
  It can also cause a failure in a worst-case scenario (think what happens if a
  tool is defined with `:=` but you don't have the tool and you don't need it either).

- The values of variables declared with `:=` depend on the order of definition.

### Memoized

```makefile
OUTPUT = $(call memoized,OUTPUT,$(shell some-command))
```

- `memoized` is a RIOT defined function that combines characteristics from
  both `=` and `:=`.
  The variable expansion will be deferred until its first usage, but further
  usage will consider it as a simply expanded variable, so it will use the already
  evaluated value. In the example `some-command` would be executed once or not
  at all (more in `makefiles/utils/variables.mk`).

[gnumake doc](https://www.gnu.org/software/make/manual/html_node/Flavors.html)

## Additional Documentation

- Deferred vs. simple expansion: http://make.mad-scientist.net/deferred-simple-variable-expansion/
- Tracking issue: [#10850](https://github.com/RIOT-OS/RIOT/issues/10850)
