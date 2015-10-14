RIOT Documentation                                                  {#mainpage}
==================

[TOC]

Overview
========

RIOT is an operating system designed for the particular requirements of Internet
of Things (IoT) scenarios. These requirements comprise a low memory footprint,
high energy efficiency, real-time capabilities, a modular and configurable
communication stack, and support for a wide range of low-power devices. RIOT
provides a microkernel, utilities like cryptographic libraries, data structures
(bloom filters, hash tables, priority queues), or a shell, different network
stacks, and support for various microcontrollers, radio drivers, sensors, and
configurations for entire platforms, e.g. Zolertia Z1 or STM32 Discovery
Boards.

The microkernel itself comprises thread management, a priority-based scheduler,
a powerful API for inter-process communication (IPC), a system timer, and
mutexes.

In order to build an application or library with RIOT, you need first to
download the source code ([Getting the source
code](https://github.com/RIOT-OS/RIOT/wiki/Introduction#wiki-getting-the-source-code)). This contains - besides the
before mentioned features - also some example applications (located in the
`examples` subdirectory) and a sample Makefile you may use for your own
project. This Makefile template shows you how to compile and link your application
against RIOT ([Compiling RIOT](https://github.com/RIOT-OS/RIOT/wiki/Introduction#wiki-compiling-riot)).

If you want to use RIOT directly with your embedded platform, you need to
install the corresponding toolchain for the deployed microcontroller ([ARM
based platforms](https://github.com/RIOT-OS/RIOT/wiki/Introduction#wiki-platforms-based-on-arm), [TI MSP430 based
platforms](https://github.com/RIOT-OS/RIOT/wiki/Introduction#wiki-platforms-based-on-ti-msp430)).

### Native RIOT - Run RIOT on your PC!

As a special platform, you will find a CPU and board called `native` in the
repository. This target allows you to run RIOT as a process on Linux on most
supported hardware platforms. Just set `BOARD = native` in your
application's Makefile, call `make`, and execute the resulting elf-file. Further
documentation about the native port can be found in `cpu/native/README.md`.

Structure
---------

![Overview](riot-overview.png)

The RIOT repository contains the following ten subdirectories:
 * boards
 * core
 * cpu
 * dist
 * doc
 * drivers
 * examples
 * pkg
 * sys
 * tests

The `boards` directory provides the configurations and initialization code for
supported IoT platforms. In `core` you can find the kernel, while `cpu`
comprises microcontroller specific implementations like startup and exception
handling code. The directory `dist` contains a template for an application's Makefile
and external utilities like the terminal program `pyterm` or a script to build
your own toolchain for ARM microcontrollers. Not very surprisingly you will find
the (doxygen) documentation in `doc` and peripheral driver code in `drivers`.
The `examples` directory provides some exemplary applications, `pkg` includes
Makefiles to integrate external libraries into RIOT, and `sys` system libraries
as well as the implementation of the network stacks which are located in
`sys/net`. Finally, the subdirectory `tests` contains test applications,
including also a few expect scripts to automatically validate some of them.

~~~~~~~~{.c}
int main()
{
    puts("Hello World!");
    return 0;
}
~~~~~~~~

Getting the source code
-----------------------

You can obtain the latest RIOT code from our [Github](https://github.com/RIOT-OS/)
repository either by [downloading the latest tarball](https://github.com/RIOT-OS/RIOT/releases)
or by cloning the [git repository](https://github.com/RIOT-OS/RIOT).

In order to clone the RIOT repository, you need the [Git revision control
system](http://git-scm.com/) and run the following command:

~~~~~~~~{.sh}
git clone git://github.com/RIOT-OS/RIOT.git
~~~~~~~~

The repository contains the kernel, support for different CPUs and platforms, device
drivers, system libraries, and network stack implementations. Inaddition it comprises
various example applications to demonstrate the usage ofsome important features.

It also provides you with useful tools like a terminal program and scripts to
setup a toolchain.

Compiling RIOT
--------------
Depending on the hardware you want to use, you need to first install a
corresponding toolchain:

* [ARM-based platforms](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM)
* [TI MSP430](https://github.com/RIOT-OS/RIOT/wiki/Family:-MSP430)
* [Atmel ATmega](https://github.com/RIOT-OS/RIOT/wiki/Family%3A-ATmega)
* [native](https://github.com/RIOT-OS/RIOT/wiki/Family:-native)

### Create an application
Once you have set up the toolchain, you can create your own application. Apart from
the C file(s) containing your source code you need a Makefile. A template
Makefile is available in the `dist` folder of the
[RIOT repository](https://github.com/RIOT-OS/RIOT).

Within your application's Makefile, you can define the target hardware as well as
the modules you want to use.

Unless specified otherwise, make will create an elf-file as well as an Intel
hex file in the `bin` folder of your application directory.

Special features
================

The build system
----------------

RIOT uses GNU make as build system. The simplest way to compile and link an
application (or library) with RIOT, is to set up a Makefile providing
at least the following variables:

 * `APPLICATION`: should contain the (unique) name of your application
 * `BOARD`: specifies the platform the application should be build for by default
 * `RIOTBASE`: specifies the path to your copy of the RIOT repository
   (note, that you may want to use `$(CURDIR)` here, to give a relative path)

Additionally it has to include the `Makefile.include`, located in RIOT's root
directory. You can use Make's `?=` operator in order to allow overwriting
variables from the command line. For example, you can easily specify the target
platform, using the sample Makefile, by invoking make like this:

~~~~~~~~{.sh}
make BOARD=iotlab-m3
~~~~~~~~

Besides typical targets like `clean`, `all`, or `doc`, RIOT provides the special
targets `flash` and `term` to invoke the configured flashing and terminal tools
for the specified platform. These targets use the variable `PORT` for the serial
communication to the device. Neither this variable nor the targets `flash` and
`term` are mandatory for the native port.

For the native port, `PORT` has a special meaning: it is used to identify the tap
interface if the @ref netdev2_tap module is used. The target `debug` can be used
to invoke a debugger on some platforms. For the native port the additional targets
such as `all-valgrind` and `valgrind` exist. Refer to `cpu/native/README.md` for
additional information

Some RIOT directories contain special Makefiles like `Makefile.base`,
`Makefile.include` or `Makefile.dep`. The first one can be included into other
Makefiles to define some standard targets. The files called `Makefile.include`
are used in `boards` and `cpu` to append target specific information to
variables like `INCLUDES`, setting the include paths. `Makefile.dep` serves to
define dependencies.

Learn more about the build system in the
[Wiki](https://github.com/RIOT-OS/RIOT/wiki/The-Make-Build-System)

Including modules
-----------------

By default a RIOT application comprises only the applications' code itself, the kernel,
and platform specific code. In order to use additional modules, such as a
particular device driver or a system library, you have to append the modules'
names to the USEMODULE variable. For example, to build an application using the SHT11
temperature sensor and UDP/IPv6 network stack, your Makefile needs to contain
these lines:

~~~~~~~~{.mk}
USEMODULE += sht11
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
~~~~~~~~

To contribute a new module to RIOT, your module's Makefile needs to set the
variable `MODULE` to a unique name. If the module depends on other modules, this
information needs to be added to RIOT's `Makefile.dep`.

The main function
-----------------

After the board is initialized, RIOT starts two threads: the idle thread and the
main thread. The idle thread has the lowest priority and will run, whenever no
other thread is ready to run. It will automatically use the lowest possible
power mode for the device. The main thread - configured with a default priority
that is right in the middle between the lowest and the highest available
priority - is the first thread that runs and calls the main function. This
function needs to be defined by the application.

Choosing the right stack size
-----------------------------

Choosing the right stack size for a new thread is not an easy, but a very
crucial task. Since memory is usually a scarce resource in IoT scenarios,
one must be careful not to assign too much stack to a single thread.
However, if you allocate too little memory for a stack, your application
will probably crash. The minimum stack size depends also on some RIOT
internal structs and is hardware dependent. In order to help developers
finding the right stack size, RIOT defines some typical stack sizes in
`cpu_conf.h` (which should be provided by the implementation for all
supported MCUs). The constants for these stack sizes are

* `THREAD_STACKSIZE_IDLE`
* `THREAD_STACKSIZE_DEFAULT`
* `THREAD_EXTRA_STACKSIZE_PRINTF`
* `THREAD_STACKSIZE_MAIN`

and can be used by including `kernel.h`. ARM based platforms additionally
define `THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT`, because newlib's `printf`
implementation uses more memory for printing floating point numbers.

`THREAD_STACKSIZE_IDLE` is the stack size for the idle thread and
probably the smallest sensible stack size. `THREAD_STACKSIZE_DEFAULT`
is a default size for any typical thread, *not* using `printf`.
`THREAD_EXTRA_STACKSIZE_PRINTF` defines additional stack space needed if the
thread needs to call printf (which requires additional memory when using
newlib). `THREAD_STACKSIZE_MAIN` is the stack size for the main thread
and probably a good size for your application. (Note, that on most
non-newlib dependent platforms this will probably equal
`THREAD_STACKSIZE_DEFAULT`.)

The IPC
-------

Like any microkernel system, RIOT has an IPC API that enables data exchange
between modules or a single module and the kernel. This API is documented in
@ref core_msg. The IPC can be used in several ways, such as synchronous or
asynchronous, blocking or non-blocking, with or without a message queue. In
the default case, a thread does not have a message queue. Hence, messages
sent in a non-blocking manner are lost, when the target thread is not in
receive mode. A thread may set up a message queue using the msg_init_queue()
function, but has to provide the memory for this queue itself.

Auto-inititialization
---------------------

Most modules require initialization before they can be used. In some cases the
initialization function does not require a parameter. For these modules the
@ref sys_autoinit feature is included automatically. It calls all module
initialization functions with a `void` parameter just before the main thread
gets executed. You can deactivate this behavior (e.g. for testing) by adding
the line

~~~~~~~~{.mk}
DISABLE_MODULE += auto_init
~~~~~~~~

to your Makefile.

Networking
----------
@todo

Community
=========

Whether you are looking for help with writing an application for RIOT, want to
learn more about it, or just stay in the loop you are invited to join the
[RIOT-users mailing list](http://lists.riot-os.org/mailman/listinfo/users).
For developers who want to participate and contribute to the kernel development
or integrate new MCU and platform support the
[RIOT-devel mailing list](http://lists.riot-os.org/mailman/listinfo/devel) is
the right place.
