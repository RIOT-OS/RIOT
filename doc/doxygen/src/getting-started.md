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

Default configurations                                 {#default-configurations}
----------------------
When devices have a common access interface, having a default configuration to
enable them across platforms, without having to explicitly specify which modules
to include, comes in handy. For this, two pseudomodules are defined:

- `saul_default`: will enable all the drivers of sensors and actuators that are
present in the target platform.

- `netdev_default`: will enable all the drivers of network devices
present in the target platform.

Use Docker to build RIOT                           {#docker}
========================
[Docker](https://www.docker.com/) is a platform that allows packaging software into containers that can easily be run on any Linux that has Docker installed.

You can download a RIOT Docker container from the Docker Hub and then use that to build your project making use of all toolchains that we've preinstalled in the container.

Setup                                              {#docker-setup}
-----

### Installing docker

To use the RIOT docker build image, the Docker application needs to be installed on your system.
To install Docker, depending on your operating system, use `sudo apt-get install docker` or a variant.

The user on your computer requires permission to access and use docker. There are two ways to manage this:
- Your OS distribution may create a group called `docker`. If so, then adding yourself to that group (and logging out and in again) should grant you permission.
- Execute docker with sudo. This is in fact the most secure and recommended setup (see [here](https://docs.docker.com/install/linux/linux-postinstall/), [here](https://docs.docker.com/engine/security/security/#docker-daemon-attack-surface), [here](https://www.projectatomic.io/blog/2015/08/why-we-dont-let-non-root-users-run-docker-in-centos-fedora-or-rhel/) and [here](https://fosterelli.co/privilege-escalation-via-docker.html)). No extra setup steps are needed. `make` should be instructed to use `sudo` by setting `DOCKER="sudo docker"` in the command line.

Finally, download the pre-built RIOT Docker container:

```console
# docker pull riot/riotbuild
```

This will take a while. If it finishes correctly, you can then use the toolchains contained in the Docker container:
(**from the riot root**):

```console
$ docker run --rm -i -t -u $UID -v $(pwd):/data/riotbuild riot/riotbuild ./dist/tools/compile_test/compile_test.py
```

# Usage

The RIOT build system provides support for using the Docker container to build RIOT projects, so you do not need to type the long docker command line every time:

(**from the directory you would normally run make, e.g. examples/default**)

```console
$ make BUILD_IN_DOCKER=1
```

If your user does not have permissions to access the Docker daemon:

```console
$ make BUILD_IN_DOCKER=1 DOCKER="sudo docker"
```

to always use Docker for building, set `BUILD_IN_DOCKER=1` (and if necessary `DOCKER="sudo docker"`) in the environment:

```console
$ export BUILD_IN_DOCKER=1
```

running make without specifying `BUILD_IN_DOCKER=1` will still use Docker (because of the environment variable)

Troubleshooting                                    {#docker-troubleshooting}
---------------

On some Ubuntu versions a make with `BUILD_IN_DOCKER=1` can't resolve the host name of for example github.com. To fix this add the file `/etc/docker/daemon.json` with the address of your DNS Server.

Generating `compile_commands.json` e.g. for code completion in IDEs
===================================================================

A `compile_commands.json` for the selected board can be generated by running inside the application
folder the following:

```console
$ make compile-commands
```

This target will honor the variables controlling the build process such as `BOARD`, `TOOLCHAIN`,
`DEVELHELP`, etc. just like the usual build process. This works without actual compilation. By
default, the `compile_commands.json` is placed in the RIOT base directory. This behavior can be
overwritten using the `COMPILE_COMMANDS_PATH` variable by specifying the full absolute path
(including file name) of the `compile_commands.json` instead.

***Note:*** By default, the built-in include search directories of GCC will be explicitly added
and flags incompatible with `clangd` will be dropped. This will allow using `clangd` as language
server out of the box. If this is not desired, run `export COMPILE_COMMANDS_FLAGS=""` to turn
modification of the compile commands off. For a list of available flags, run
`./dist/tools/compile_commands/compile_commands.py --help` in the RIOT base directory.

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
