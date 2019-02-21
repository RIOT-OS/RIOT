Creating modules                                            {#creating-modules}
================
Modules in RIOT are well-defined units of code that provide a set of features
to your application. This includes also drivers and to a certain extent ports
for CPUs and boards (with some exceptions, see <!-- @ref porting-guide --> the
porting guide for further information).

The general structure                                  {#the-general-structure}
=====================
Like @ref creating-an-application "applications", modules are directories
containing source files and a Makefile. Additionally their API can be defined
in one or more header files, residing in the include path of their
super-module.

E.g. the @ref sys_shell module is implemented in `sys/shell` and defines its
API in `sys/include/shell.h` and the @ref drivers_isl29020 driver is
implemented in `drivers/isl29020` and defines its API in
`drivers/include/isl29020.h`.

A module's Makefile just needs to include `Makefile.base` in the RIOT
repository:

~~~~~~~~~~~~~~~~~~~ {.mk}
include $(RIOTBASE)/Makefile.base
~~~~~~~~~~~~~~~~~~~

If your module's name differs from the name of the directory it resides in you
need to set the `MODULE` macro in addition.

When compiled a module always provides a `MODULE_<MODULENAME>` macro to the
system. This way, other modules can check if the module is available in the
current configuration or not.

Modules can be used by adding their name to the `USEMODULE` macro of your
application's Makefile.

### Pitfalls ###

The `MODULE` name should be unique or build breaks as modules overwrite the
same output file. This might for example lead to `undefined reference to` errors
in the linker which can be hard to track down.

This problem happened in the past for:

 * Packages root directory (libfixmath/u8g2)
 * boards/cpu/periph and their common boards/cpu/periph

Note: even if all boards and cpus implement the `board` and `cpu` modules, only
      one is used in an application so there is no conflict.


Module dependencies
===================
Your module may depend on other modules to minimize code duplication. These
dependencies are defined in `Makefile.dep` with the following syntax:

~~~~~~~~~~~~~~~~~~~ {.mk}
ifneq (,$(filter your_module,$(USEMODULE))) # if module in USEMODULE
  USEMODULE += dep1                         # add dependencies to USEMODULE
  USEMODULE += dep2
endif
~~~~~~~~~~~~~~~~~~~

Note, that `Makefile.dep` is processed only once so you have to take care to
add the dependency block for your module *before* your dependencies pull in
their dependencies.

Modules outside of RIOTBASE                      {#modules-outside-of-riotbase}
===========================
Modules can be defined outside `RIOTBASE`. In addition to add it to `USEMODULE`
the user needs to add the module path to `EXTERNAL_MODULE_DIRS`.

The external module can optionally define the following files:
* `Makefile.include` file to set global build configuration like `CFLAGS` or add
  API headers include paths to the `USEMODULE_INCLUDES` variable.
* `Makefile.dep` file to set module dependencies

An example can be found in
[`tests/external_module_dirs`](https://github.com/RIOT-OS/RIOT/tree/master/tests/external_module_dirs)

Pseudomodules                                                  {#pseudomodules}
=============
Pseudomodules are modules that do not have any code. Their main use cases are
to provide base information for dependencies to other modules or information to
the code base via the `MODULE_<MODULENAME>` macro. Pseudomodules can provide
header files too, if need be. To create a pseudomodule just add its name to the
`PSEUDOMODULES` macro in `Makefile.pseudomodules`.
