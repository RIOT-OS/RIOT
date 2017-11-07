# Emulation using Emul8

## Introduction
[Emul8](http://emul8.org) is an open source embedded systems emulation framework. It offers support for a broad range of processors, including ARM, ARM Cortex, x86 and more. In addition it is an extensible framework that can also emulate peripherals.

## Installation

### From package
At the time of writing (November 2017), no packages are available.

### From source
Follow the installation instructions on their [GitHub](https://github.com/emul8/emul8#installation) page.

After compilation is successful, ensure that `emul8` is available on your `PATH`. One way to do so, is symlinking `path/to/emul8/repository/run.sh` to `/usr/local/lib/emul8`.

### Testing
After installation, verify if Emul8 is working using `emul8 --help`. You should be presented with a help screen.

## Usage
From within RIOT-OS, you can use `make emulate` to start emulation. It expects a board definition file in `boards/<BOARD>/dist/board.emul8`.

The board definition file will tell Emul8 how to setup an emulation session. The application binary file (`*.elf`) is available using the variable `$image_file`.

For an example, refer to `boards/cc2538dk/dist/board.emul8`.
