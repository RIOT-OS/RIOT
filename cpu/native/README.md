Valgrind Support
================

Rebuild your application using the all-valgrind target like this:

    make -B clean all-valgrind

That way native will tell Valgrind about RIOT's stacks and prevent
Valgrind from reporting lots of false positives.
The debug information flag `-g` is not strictly necessary, but passing
it allows Valgrind to tell you precisely which code triggered the error.

To run your application run:

    make term-valgrind

All this does is run your application under Valgrind.
Now Valgrind will print some information whenever it detects an
invalid memory access.

In order to debug the program when this occurs you can use the targets
debug-valgrind-server and debug-valgrind. Therefore, you need to open two
terminals and run:

    make debug-valgrind-server

in the first one and run:

    make debug-valgrind

in the seconde one. This starts per default gdb attached to valgrinds gdb
server (vgdb).

Network Support
===============

If you compile RIOT for the native cpu and include the `netdev2_tap`
module, you need to specify a network interface like this:

    make term PORT=tap0

**Please note:** in case you're using RIOT's default network stack, the GNRC
stack, you may also use `gnrc_netdev_default` module and also add
`auto_init_gnrc_netif` in order to automatically initialize the interface.


Setting Up A Virtual Network
============================

There is a shellscript in RIOT/dist/tools/tapsetup called `tapsetup` which you
can use to create a network of tap interfaces.

Usage:
To create a bridge and two (or count at your option) tap interfaces:

    ../../dist/tools/tapsetup/tapsetup [-c [<count>]]

On OSX you need to start the RIOT instance at some point during the script's
execution. The script will instruct you when to do that.

To delete the bridge and all tap interfaces:

    ../../dist/tools/tapsetup/tapsetup -d

For OSX you **have** to run this after killing your RIOT instance and rerun
`../../dist/tools/tapsetup [-c [<count>]]` before restarting.

**Please note:** If you want to communicate between RIOT and your host
operating system, you must not use the `tapsetup` script, but create and
activate the tap interface manually. On Linux you can do so, by calling

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up


Daemonization
=============

You can daemonize a riot process. This is useful for larger networks.
Valgrind will fork along with the riot process and dump its output in
the terminal.

Usage:

    ./bin/native/default.elf -d

Compile Time Options
====================

Compile with

    CFLAGS=-DNATIVE_AUTO_EXIT make

to exit the riot core after the last thread has exited.
