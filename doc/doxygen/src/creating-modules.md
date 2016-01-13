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
include $(RIOTBASE)/Makefile
~~~~~~~~~~~~~~~~~~~

If your module's name differs from the name of the directory it resides in you
need to set the `MODULE` macro in addition.

When compiled a module always provides a `MODULE_<MODULENAME>` macro to the
system. This way, other modules can check if the module is available in the
current configuration or not.

Modules can be used by adding their name to the `USEMODULE` macro of your
application's Makefile.

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
the user needs to add the path to the module to `EXTERNAL_MODULES` and add the
include path to the API definitions to `INCLUDES`.

Pseudomodules                                                  {#pseudomodules}
=============
Pseudomodules are modules that do not have any code. Their main use cases are
to provide base information for dependencies to other modules or information to
the code base via the `MODULE_<MODULENAME>` macro. Pseudomodules can provide
header files too, if need be. To create a pseudomodule just add its name to the
`PSEUDOMODULES` macro in `Makefile.pseudomodules`.
