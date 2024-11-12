Creating an application                              {#creating-an-application}
=======================

[TOC]

To create your own application you need to create a directory containing one or
multiple C file(s) with your source code and a Makefile. A template Makefile is
available in the `dist` folder of the
[RIOT repository](https://github.com/RIOT-OS/RIOT).

The main function                                          {#the-main-function}
=================
After the board is initialized, RIOT starts two threads: the idle thread and
the main thread. The idle thread has the lowest priority and will run whenever
no other thread is ready to run. It will automatically use the lowest possible
power mode for the device. The main thread - configured with a default priority
that is right in the middle between the lowest and the highest available
priorities - is the first thread that runs and calls the `main()` function.
This function needs to be defined in the source code of the application
(typically located in `main.c`).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>

int main(void)
{
    puts("Hello World!");
    return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The application's Makefile                         {#the-applications-makefile}
==========================
The minimal Makefile                                    {#the-minimal-makefile}
--------------------
At minimum the Makefile of an application (see @ref getting-started) needs to
define the following macros:
 * `APPLICATION`: should contain the name of your application
 * `RIOTBASE`: specifies the path to your copy of the RIOT repository (note
   that you may want to use `$(CURDIR)` here, to give a relative path)

The `BOARD` macro is also required and recommended to be set to `native` by
default, but is recommended to be overridable with the `?=` operator.
Additionally, it is required to include the `Makefile.include` from the
`RIOTBASE`.

~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
# Set the name of your application:
APPLICATION = foobar

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include
~~~~~~~~~~~~~~~~~~~~~~~~~~~

How to handle unsupported boards?                  {#handle-unsupported-boards}
---------------------------------
Sometimes it is necessary to exclude boards because they don't provide a
required functionality or don't have sufficient memory. RIOT's build system
looks for the macros `BOARD_BLACKLIST`, `BOARD_WHITELIST`, and
`BOARD_INSUFFICIENT_MEMORY`. Any board name that is not included in
`BOARD_WHITELIST` will issue a message that one has to expect errors if they
build the application for the board referred by that name. The list can also be
used by a CI system to not build the application for this board at all. A board
that is included in `BOARD_BLACKLIST` will show the same behavior. The build
system evaluates `BOARD_WHITELIST` first and then `BOARD_BLACKLIST`. The
`BOARD_INSUFFICIENT_MEMORY` macro is similar to `BOARD_BLACKLIST` but will
build in any case. A CI system can use the information provided by the
`BOARD_INSUFFICIENT_MEMORY` macro to skip the linking step in the build
process, since some linkers will issue an error if the code won't fit the
target board's flash memory or RAM.

Including modules                                          {#including-modules}
-----------------
By default a RIOT application comprises only of the applications' code itself,
the kernel, and platform specific code. In order to use additional modules,
such as a particular [device driver](@ref drivers) or [a system library](@ref sys)
(including [networking capabilities](@ref net)), you have to append the modules'
names to the USEMODULE variable. For example, to build an application using the
SHT11 temperature sensor and UDP/IPv6 functionalities of the GNRC network stack,
your Makefile needs to contain these lines:

~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
USEMODULE += sht11
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modules typically pull in all required dependencies.

## Including source files in subfolders

By default, all source files in an application's (or any RIOT module's) directory
are automatically compiled as part of the application. In order to organize source
code in a directory structure, two different approaches can be used:

1. Make each subdirectory a separate RIOT module with a unique name inside its
Makefile, either by adding the directory's path to `DIRS` or with the [out-of-tree
module support](#external-modules).
2. Add the source files within subdirectories to `SRC`, either explicitly or with
Makefile wildcards.

Both approaches are illustrated and explained in `examples/subfolders`.


# Helper tools

To help you start writing an application within RIOT, the build system provides
the `generate-example` `generate-test` make targets. These targets are wrappers
around the [riotgen](https://pypi.org/project/riotgen/) command line tool and
are helpful when starting to implement an application: all required files are
generated with copyright headers, doxygen groups, etc, so you can concentrate
on the module implementation.
For applications, the `Makefile` is generated with the dependencies (modules,
packages, required features) included.

**Usage:**

To generate an example application, e.g in the `examples` directory, from the
RIOT base directory, run:
```
make generate-example
```

To generate a test application, e.g in the `tests` directory, from the
RIOT base directory, run:
```
make generate-test
```

Then answer a few questions about the application:
- Application name: enter a name for your application. It will be used as both
  the name of the application directory under `examples` or `tests` and by
  the build system module (set in the `APPLICATION` variable).
- Application brief description: Describe in one line what is this application
  about.
- Target board: select the default target board. The value is set to `native`
  by default.
- Modules: give the list of dependency modules, separated by commas. For
  example: ztimer,fmt
- Packages: give the list of dependency packages, separated by commas.
- Features required: give the list of CPU features (`periph_*`, etc) required
  by the application, all separated by commas.

Other global information (author name, email, organization) should be retrieved
automatically from your git configuration.

Once completed, the application files are either located in
`examples/<application name>` or `tests/<application name>` depending on the
target used.

**Testrunner:** when using the `make generate-test`, you can also automatically
add a testrunner Python script. Just answer 'y' when prompted.

# Creating an out of tree application structure

Applications written for RIOT do not have to reside in the RIOT tree. Out of
tree applications, modules and boards are supported.

For a full application with custom board and modules, the following directory
tree can be used:

```
├── apps
│   └── my_app
│       └── Makefile
├── boards
│   └── my_board
├── modules
│   └── my_module
│       ├── include
│       │   └── my_module.h
│       ├── Makefile
│       ├── Makefile.include
│       └── my_module.c
└── RIOT
```

In this example tree, the `apps` directory contains a collection of applications
for the project. The modules directory could contain extra modules for the
applications.

The make file inside the application needs at least the following as bare minimum:

```
APPLICATION = my_app
PROJECT_BASE ?= $(CURDIR)/../..
RIOTBASE ?= $(PROJECT_BASE)/RIOT

# Optionally, provide paths to where external boards and/or modules
# reside, so that they can be included in the app
EXTERNAL_MODULE_DIRS += $(PROJECT_BASE)/modules
EXTERNAL_BOARD_DIRS += $(PROJECT_BASE)/boards

include $(RIOTBASE)/Makefile.include
```

The `RIOTBASE` variable tells the build system where to find the RIOT source
tree and to need to point to the RIOT source tree used for the application for
the application to work.

The RIOT directory contains the sources of RIOT here. This can be either a
direct checkout of the sources or a git submodule, whichever has your
preference.

If your project has separate modules or separate boards, these can be contained
inside a modules os boards directory. The RIOT build system has both
`EXTERNAL_MODULE_DIRS` and `EXTERNAL_BOARD_DIRS` variables to specify
directories that contain extra modules and extra boards.

## External Boards

External boards can be ported in an identical way as porting a regular board to
RIOT, see @ref porting-boards.

One approach can be to copy over an existing board and modify it to suit the
needs. Boards in the RIOT tree can be included and used as dependency in the
custom boards. In case you connect additional hardware to an upstream board
(such as e.g. an Arduino shield) or you require a different hardware
configuration (e.g. configuring some of the pins configured as ADC as
additional PWM outputs instead) a copy of the upstream board that is then
customized to the application needs is the best course of action.

## External Modules                                         {#external-modules}

Similar to the external boards, external modules can be written in a similar way
as regular in-tree modules.

One modification is the include directory inside the module directory. For this
include directory to be added to the include path during compilation, the
following snippet is required in the modules `Makefile.include`:

```
USEMODULE_INCLUDES_my_module := $(LAST_MAKEFILEDIR)/include
USEMODULE_INCLUDES += $(USEMODULE_INCLUDES_my_module)
```

Note that the make variable (here `USEMODULE_INCLUDES_my_module`) must be unique
for every module to make this work. Including the module name here is usually
sufficient.

## Extra Makefile Scaffolding

A bit of extra, but optional, Makefile scaffolding can help to keep the project
easy to maintain. An extra `Makefile.include` in the root directory of the
project that sets the necessary variables can help to deduplicate settings.
This includes the `RIOTBASE` variable and the include to the RIOT
`Makefile.include`:

```
EXTERNAL_MODULE_DIRS += $(PROJECT_BASE)/modules
EXTERNAL_BOARD_DIRS += $(PROJECT_BASE)/boards
RIOTBASE ?= $(PROJECT_BASE)/RIOT
include $(RIOTBASE)/Makefile.include
```
Applications then just have to set the `PROJECT_BASE` variable and include this
makefile and don't have to each add the external board and module directories.

The application makefile would then look like this:
```
APPLICATION = my_app
PROJECT_BASE ?= $(CURDIR)/../..
include $(PROJECT_BASE)/Makefile.include
```
