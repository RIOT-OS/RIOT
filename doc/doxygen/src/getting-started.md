Getting started                                              {#getting-started}
===============

[TOC]

@deprecated See [Setup Guide](https://guide.riot-os.org/getting-started/installing/#architecture-specific-requirements) on the RIOT Guide Site for the latest information. This section will be removed after release 2025.11.

The build system                                            {#the-build-system}
----------------

@deprecated See [The Build System](https://guide.riot-os.org/build-system/build_system/) on the RIOT Guide Site for the latest information. This section will be removed after release 2025.11.


Building and executing an example           {#building-and-executing-an-example}
---------------------------------
@deprecated See [Building and executing an example](https://guide.riot-os.org/getting-started/building_example/) on the RIOT Guide Site for the latest information. This section will be removed after release 2025.11.

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

@deprecated See [Setup Guide](https://guide.riot-os.org/getting-started/installing/) or [Build in Docker](https://guide.riot-os.org/build-system/build-in-docker/) on the RIOT Guide Site for the latest information. This section will be removed after release 2025.11.

Generating compile_commands.json e.g. for code completion in IDEs
=================================================================

See [Setup Guide](https://guide.riot-os.org/getting-started/installing/#using-vs-code-for-development) for a guide on how to set up VS Code for RIOT development using the `compile_commands.json` file.

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

A detailed example can be found in `examples/networking/gnrc/gnrc_networking`.
