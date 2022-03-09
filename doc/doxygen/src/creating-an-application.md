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
