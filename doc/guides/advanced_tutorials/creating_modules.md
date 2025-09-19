---
title: Creating Modules
description: Guide on how to create modules in RIOT-OS
---

Modules in RIOT are well-defined units of code that provide a set of features
to your application. This includes also drivers and to a certain extent ports
for CPUs and boards (with some exceptions, see the
[porting guide](/advanced_tutorials/porting_boards/) for further information).

## The General Structure

Like [applications](/advanced_tutorials/creating_application/), modules are directories
containing source files and a Makefile. Additionally their API can be defined
in one or more header files, residing in the include path of their
super-module.

E.g. the [Shell](https://doc.riot-os.org/group__sys__shell.html) module
is implemented in `sys/shell` and defines its
API in `sys/include/shell.h` and the
[ISL29020 light sensor](https://doc.riot-os.org/group__drivers__isl29020.html)
driver is implemented in `drivers/isl29020`
and defines its API in `drivers/include/isl29020.h`.

A module's Makefile just needs to include `Makefile.base` in the RIOT
repository:

```makefile
include $(RIOTBASE)/Makefile.base
```

If your module's name differs from the name of the directory it resides in you
need to set the `MODULE` macro in addition.

When compiled a module always provides a `MODULE_<MODULENAME>` macro to the
system. This way, other modules can check if the module is available in the
current configuration or not.

Modules can be used by adding their name to the `USEMODULE` macro of your
application's Makefile.

### Pitfalls

The `MODULE` name should be unique or build breaks as modules overwrite the
same output file. This might for example lead to `undefined reference to` errors
in the linker which can be hard to track down.

This problem happened in the past for:

 * Packages root directory (libfixmath/u8g2)
 * boards/cpu/periph and their common boards/cpu/periph

Note: even if all boards and cpus implement the `board` and `cpu` modules, only
      one is used in an application so there is no conflict.


## Module Dependencies

Your module may depend on other modules to minimize code duplication. These
dependencies are defined in `Makefile.dep` with the following syntax:

```makefile
ifneq (,$(filter your_module,$(USEMODULE))) # if module in USEMODULE
  USEMODULE += dep1                         # add dependencies to USEMODULE
  USEMODULE += dep2
endif
```

Note, that `Makefile.dep` is processed only once so you have to take care to
add the dependency block for your module *before* your dependencies pull in
their dependencies.

## Modules Outside of RIOTBASE

Modules can be defined outside of `RIOTBASE`. In addition to adding it to `USEMODULE`
the user needs to add the directory (or directories) containing external modules
to `EXTERNAL_MODULE_DIRS`.

External modules can optionally define the following files:
* `Makefile.include` file to set global build configuration like `CFLAGS` or add
  API headers include paths to the `USEMODULE_INCLUDES` variable.
* `Makefile.dep` file to set module dependencies

:::note
The name of an external module must be unique (both in regard to other
external modules, as well to native RIOT modules). Additionally, the
directory containing the module must match the module name, e.g.
module `foo` must be located in `<PATH_IN_EXTERNAL_MODULE_DIRS>/foo`.
:::

An example can be found in
[`tests/build_system/external_module_dirs`](https://github.com/RIOT-OS/RIOT/tree/master/tests/build_system/external_module_dirs)

## Pseudomodules

Pseudomodules are modules that are not static libraries, i.e. do not generate a
`<module name>.a` file.

To create a pseudomodule just add its name to `makefiles/pseudomodules.inc.mk`
with `PSEUDOMODULES += <modulename>` in alphabetical order.

A Pseudomodule may or may not have a source file associated with it. To make the
distinction between them we will refer to those that don't as true-Pseudomodules.

The main use case for true-Pseudomodules is to provide base information for
dependencies to other modules or information to the code base via the
`MODULE_<MODULENAME>` macro.

Pseudomodules with source code exist under a "real" `MODULE` since they will
generate a `<pseudomodule_name>.o` file grouped under that `MODULE`s
`<module_name>.a` file.

These modules appear in RIOT under two forms:

1. Conditionally included source files:

  ```
    foo/
    |----foo_bar.c
    |----foo.c
    |----Makefile
  ```

In `foo/Makefile` you add the source file to the `SRC` variable, conditioned on
the Pseudomodule inclusion

  ```makefile
  ifneq (,$(filter foo_bar,$(USEMODULE)))
    SRC += foo_bar.c
  endif
  ```

See `sys/net/ble/skald` for an example in code.

2. Using the `SUBMODULES` mechanism:

  ```
    foo/
    |----spam.c
    |----ham.c
    |----eggs.c
    |----Makefile
  ```

  ```makefile
  # make all code end up in "foo_bar.a", this can be any name
  MODULE := foo_bar

  # ensure that "foo_ham" or "bar_foo_ham" builds "foo_ham.c".
  BASE_MODULE := foo

  # list of source files that are not SUBMODULES
  SRC := spam.c

  # enable submodules by setting SUBMODULES = 1
  SUBMODULES = 1
  ```

When using `SUBMODULES` in a `MODULE` all `SRC` file excluded from `foo/Makefile`
will be considered `SUBMODULES`. In the example above `ham.c` and `eggs.c`.
These source files will be conditionally included depending if the modules have
been added, i.e. `USEMODULE += foo_ham foo_eggs` (it's the same as case 1 but
handled automatically in `Makefile.base`).

The `SUBMODULES` mechanism is more flexible since `BASE_MODULE` allows matching
the only parts of compounded module names and only match against part of that name.

See `sys/ztimer/Makefile` for an example in code.

`SUBMODULES` can also be true-pseudomodules, or become one by conditionally excluding
the source files by adding them to `SUBMODULES_NO_SRC`.

# Helper Tools

To help you start writing a module, the RIOT build system provides the
`generate-module` make target. It is a wrapper around the
[riotgen](https://pypi.org/project/riotgen/) command line tool that is helpful
when starting to implement a module: all required files are generated with
copyright headers, doxygen groups, etc, so you can concentrate on the module
implementation.
The module source files are created in the `sys` directory.

## Usage

From the RIOT base directory, run:
```sh
make generate-module
```
Then answer a few questions about the driver:
- Module name: enter a name for your module. It will be used as both the name
  of the module directory under sys, where the source files are created, and
  the build system module (used with `USEMODULE`).
- Module doxygen name: Enter the name of module, as displayed in the
  Doxygen documentation.
- Brief doxygen description: Describe in one line what is this module about.

Other global information (author name, email, organization) should be retrieved
automatically from your git configuration.

Once completed, the module files are located in
`sys/<module name>/<module name>.c` and `sys/include/<module name>.h`.
