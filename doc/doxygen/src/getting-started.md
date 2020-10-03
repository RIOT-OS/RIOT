Getting started                                              {#getting-started}
===============

[TOC]

Downloading RIOT code                                  {#downloading-riot-code}
=====================
You can obtain the latest RIOT code from
our [Github](https://github.com/RIOT-OS/) repository either by
[downloading the latest tarball](https://github.com/RIOT-OS/RIOT/releases) or
by cloning the [git repository](https://github.com/RIOT-OS/RIOT).

In order to clone the RIOT repository, you need the
[Git revision control system](http://git-scm.com/) and run the following
command:

~~~~~~~~ {.sh}
git clone git://github.com/RIOT-OS/RIOT.git
~~~~~~~~

Compiling RIOT                                                {#compiling-riot}
==============

Setting up a toolchain                                {#setting-up-a-toolchain}
----------------------
Depending on the hardware you want to use, you need to first install a
corresponding toolchain. The Wiki on RIOT's Github page contains a lot of
information that can help you with your platform:

* [ARM-based platforms](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM)
* [TI MSP430](https://github.com/RIOT-OS/RIOT/wiki/Family:-MSP430)
* [Atmel ATmega](https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega)
* [native](https://github.com/RIOT-OS/RIOT/wiki/Family:-native)

The build system                                            {#the-build-system}
----------------
RIOT uses [GNU make](https://www.gnu.org/software/make/) as build system. The
simplest way to compile and link an application with RIOT, is to set up a
Makefile providing at least the following variables:

 * `APPLICATION`: should contain the (unique) name of your application
 * `BOARD`: specifies the platform the application should be built for by
   default
 * `RIOTBASE`: specifies the path to your copy of the RIOT repository (note,
   that you may want to use `$(CURDIR)` here, to give a relative path)

Additionally it has to include the `Makefile.include`, located in RIOT's root
directory:

~~~~~~~~ {.mk}
# a minimal application Makefile
APPLICATION = mini-makefile
BOARD ?= native
RIOTBASE ?= $(CURDIR)/../RIOT

include $(RIOTBASE)/Makefile.include
~~~~~~~~

You can use Make's `?=` operator in order to allow overwriting
variables from the command line. For example, you can easily specify the target
platform, using the sample Makefile, by invoking make like this:

~~~~~~~~ {.sh}
make BOARD=iotlab-m3
~~~~~~~~

Besides typical targets like `clean`, `all`, or `doc`, RIOT provides the
special targets `flash` and `term` to invoke the configured flashing and
terminal tools for the specified platform. These targets use the variable
`PORT` for the serial communication to the device. Neither this variable nor
the targets `flash` and `term` are mandatory for the native port.

For the native port, `PORT` has a special meaning: it is used to identify the
tap interface if the `netdev_tap` module is used. The target `debug` can be
used to invoke a debugger on some platforms. For the native port the additional
targets such as `all-valgrind` and `valgrind` exist. Refer to
`cpu/native/README.md` for additional information

Some RIOT directories contain special Makefiles like `Makefile.base`,
`Makefile.include` or `Makefile.dep`. The first one can be included into other
Makefiles to define some standard targets. The files called `Makefile.include`
are used in `boards` and `cpu` to append target specific information to
variables like `INCLUDES`, setting the include paths. `Makefile.dep` serves to
define dependencies.

Unless specified otherwise, make will create an elf-file as well as an Intel
hex file in the `bin` folder of your application directory.

Learn more about the build system in the
[Wiki](https://github.com/RIOT-OS/RIOT/wiki/The-Make-Build-System)

Building and executing an example           {#building-and-executing-an-example}
---------------------------------
RIOT provides a number of examples in the `examples/` directory. Every example
has a README that documents its usage and its purpose. You can build them by
typing

~~~~~~~~ {.sh}
make BOARD=samr21-xpro
~~~~~~~~

or

~~~~~~~~ {.sh}
make all BOARD=samr21-xpro
~~~~~~~~

into your shell.

To flash the application to a board just type

~~~~~~~~ {.sh}
make flash BOARD=samr21-xpro
~~~~~~~~

You can then access the board via the serial interface:

~~~~~~~~ {.sh}
make term BOARD=samr21-xpro
~~~~~~~~

If you are using multiple boards you can use the `PORT` macro to specify the
serial interface:

~~~~~~~~ {.sh}
make term BOARD=samr21-xpro PORT=/dev/ttyACM1
~~~~~~~~

Note that the `PORT` macro has a slightly different semantic in `native`. Here
it is used to provide the name of the TAP interface you want to use for the
virtualized networking capabilities of RIOT.

We use `pyterm` as the default terminal application. It is shipped with RIOT in
the `dist/tools/pyterm/` directory. If you choose to use another terminal
program you can set `TERMPROG` (and if need be the `TERMFLAGS`) macros:

~~~~~~~~ {.sh}
make -C examples/gnrc_networking/ term \
    BOARD=samr21-xpro \
    TERMPROG=gtkterm \
    TERMFLAGS="-s 115200 -p /dev/ttyACM0 -e"
~~~~~~~~

Configuring an application                         {#configuring-an-application}
--------------------------
Many modules in RIOT offer configuration options that will be considered during
compile-time.They are modeled as macros that can be overridden by the user.
Currently there are two ways of doing this: using `CFLAGS` or via
@ref kconfig-in-riot "Kconfig" (the last one is currently only possible for a
subset of modules).

For instructions on how to configure via `CFLAGS` check the
@ref config "identified compile-time configurations". To learn how to use
Kconfig in RIOT, please refer to the @ref kconfig-users-guide.

Using the native port with networking
=====================================

If you compile RIOT for the native cpu and include the `netdev_tap` module,
you can specify a network interface like this: `PORT=tap0 make term`

Setting up a tap network
------------------------

There is a shell script in `RIOT/dist/tools/tapsetup` called `tapsetup` which
you can use to create a network of tap interfaces.

*USAGE*

To create a bridge and two (or `count` at your option) tap interfaces:
~~~~~~~{.sh}
    sudo ./dist/tools/tapsetup/tapsetup [-c [<count>]]
~~~~~~~
