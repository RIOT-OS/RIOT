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

In order to debug the program when this occurs you can pass the
--db-attach parameter to Valgrind. E.g:

    valgrind --db-attach=yes ./bin/native/default.elf tap0

Now, you will be asked whether you would like to attach the running
process to gdb whenever a problem occurs.

In order for this to work under Linux 3.4 or newer, you might need to
disable the ptrace access restrictions:
As root call:

    echo 0 > /proc/sys/kernel/yama/ptrace_scope


Network Support
===============

If you compile RIOT for the native cpu and include the `native_net`
module, you need to specify a network interface like this:

    make term PORT=tap0


Setting Up A Tap Network
========================

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


Daemonization
=============

You can daemonize a riot process. This is useful for larger networks.
Valgrind will fork along with the riot process and dump its output in
the terminal.

Usage:

    ./bin/native/default.elf -d

Use UART redirection if you want to use a shell or get stderr/stdout
output with/from a daemonized process.


UART Redirection
================

You can redirect the processes' stdin/stdout/stderr by specifying
one or more options from below.

UNIX socket
-----------

To redirect stdio to a UNIX socket run:

    ./bin/native/default.elf -u -d
    RIOT pid: 18663

Attach this UNIX socket:

    nc -U /tmp/riot.tty.18663

TCP socket
----------
To redirect stdio to a TCP socket:

    ./bin/native/default.elf -t 4711 -d
    RIOT pid: 18663

Attach this TCP socket:

    nc localhost 4711

Stop the process:

    kill 18663

File for stderr
---------------
To redirect stderr to a file:

    ./bin/native/default.elf -d -e
    RIOT pid: 18663

Read from it:

    tail -f /tmp/riot.stderr.18663

File for stdout
---------------
To redirect stdout to a file:

    ./bin/native/default.elf -d -o
    RIOT pid: 18663

Read from it:

    tail -f /tmp/riot.stdout.18663


Notes
-----
The stdout redirection only writes to file while no socket connection
is established.

Socket redirection is only available when the UART module has been
compiled in.


Compile Time Options
====================

Compile with

    CFLAGS=-DNATIVE_AUTO_EXIT make

to exit the riot core after the last thread has exited.
