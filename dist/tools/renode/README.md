# Emulation using Renode

## Introduction
[Renode](http://renode.io) an open source software development framework that
lets you develop, debug and test multi-node device systems reliably, scalably
and effectively. It can easily be used to run applications on a broad range of
embedded platforms without any changes in the code itself, as if you were
running on real hardware - but with more possibilities.

## Installation

### From package
Packages for macOS, deb-based and rpm-based systems, for Windows and for Arch
Linux are available on
[GitHub](https://github.com/renode/renode/releases/latest). The minimal
required version in 1.12.

### From source
Follow the installation instructions on Renode's
[GitHub](https://github.com/renode/renode#installation) page.

After compilation is successful, ensure that `renode` is available on your
`PATH`. One way to do so, is via symlink:
`sudo ln -s path/to/renode/repository/renode /usr/local/bin/renode`.

### Testing
After installation, verify if Renode is working using `renode --help`. You
should be presented with a help screen.

## Documentation
Documentation for Renode can be found on
[Read The Docs](https://renode.readthedocs.io).

## Usage
From within RIOT-OS, you can use `make emulate` to start emulation.
By default, it expects a board script file in
`boards/<BOARD>/dist/renode/board.resc`. This can be overridden by defining
the `RENODE_BOARD_CONFIG` in the `Makefile.include`

The board script file will tell Renode how to setup an emulation session, such
as the board, CPU and the binary. For an example, see to
`boards/cc2538dk/dist/renode/board.resc`.

Several variables are exported by RIOT-OS for Renode. The following variables
can be used:

* `$BOARD` - Selected board name
* `$CPU` - Selected CPU name
* `$ELFFILE` - Full path to the image file
* `$RIOTBASE` - Full path to RIOT-OS directory
* `$RIOTBOARD` - Full path to board directory
* `$RIOTCPU` - Full path to CPU directory
