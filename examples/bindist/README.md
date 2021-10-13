# Introduction

RIOT allows for creating a "binary distribution", which can be used to ship
proprietary, compiled objects in a way that makes it possible to re-link them
against a freshly compiled RIOT.
This "binary distribution" also contains version information and md5 hashes of
a linked binary, making verification of correctness of a link possible.

This application serves as simple example for "make bindist".
It consists of an application module (bindist.a) and another example module
(abc.a).

See https://github.com/RIOT-OS/RIOT/wiki/LGPL-guide for background information.

## Instructions

Calling "make bindist" creates a folder "bindist", which only contains the
compiled and linked binary, bindist.a, abc.a and Makefiles.

In order to recompile RIOT, adjust "RIOTBASE" in Makefile to point to a RIOT
source checkout, then call "make check_bindist".

RIOT will be built as usual, but just take the pre-compiled bindist.a and
abc.a. Their source is not necessary. The resulting binary will then be
compared with the precompiled "bindist.elf" (using md5sum) and the result gets
printed. If the same RIOT source tree and build environment (compiler version,
etc.) was used, the binaries should match.

Step-by-step:

    $ cd <riot-checkout>/examples/bindist
    $ make all
    $ make bindist
    $ cd bindist
    <adjust RIOTBASE variable (../.. -> ../../..)
    $ make check_bindist

## Needed Makefile changes

In order to enable "make bindist" for your application, several things have to
be changed in the main application's Makefile.

See this application's Makefile as example.
