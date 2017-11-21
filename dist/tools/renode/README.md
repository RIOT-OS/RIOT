# Emulation using Renode

## Introduction
[Renode](http://renode.io) is a virtual development tool for multinode embedded networks (both wired and wireless) enabling a scalable workflow for building effective, tested and secure IoT systems, created by [Antmicro](http://antmicro.com/blog/2017/08/renode-press-release/).
It can easily be used to run applications on a broad range of embedded platforms without any changes in the code itself, as if you were running on real hardware - but with more possibilities.

## Installation

### From package
Packages for macOS, deb-based and rpm-based systems, for Windows and for Arch Linux are available on [GitHub](https://github.com/renode/renode/releases/latest).

### From source
Follow the installation instructions on Renode's [GitHub](https://github.com/renode/renode#installation) page.

After compilation is successful, ensure that `renode` is available on your `PATH`. One way to do so, is via symlink: `sudo ln -s path/to/renode/repository/renode /usr/local/bin/renode`.

### Testing
After installation, verify if Renode is working using `renode --help`. You should be presented with a help screen.

## Documentation
Documentation for Renode can be found on [Read The Docs](https://renode.readthedocs.io).

## Usage
From within RIOT-OS, you can use `make emulate` to start emulation. It expects a board definition file in `boards/<BOARD>/dist/board.resc`.

The board definition file will tell Renode how to setup an emulation session. The application binary file (`*.elf`) is available using the variable `$image_file`.

For an example, refer to `boards/cc2538dk/dist/board.resc`.
