examples/default
================
This application is a showcase for RIOT's hardware support. Using it
for your board, you should be able to interactively use any hardware
that is supported.

To do this, the application uses the `shell` and `shell_commands`
modules and all the driver modules each board supports.

`shell` is a very simple interactive command interpreter that can be
used to call functions.  Many of RIOT's modules define some generic
shell commands. These are included via the `shell_commands` module.

Additionally, the `ps` module which provides the `ps` shell command is
included.

Usage
=====

Build, flash and start the application:
```
export BOARD=your_board
make
make flash
make term
```

The `term` make target starts a terminal emulator for your board. It
connects to a default port so you can interact with the shell, usually
that is `/dev/ttyUSB0`. If your port is named differently, the
`PORT=/dev/yourport` variable can be used to override this.


Example output
==============

The shell commands come with online help. Call `help` to see which commands
exist and what they do.

Running the `help` command on an msba2:
```
2014-05-06 13:14:38,508 - INFO # > help
2014-05-06 13:14:38,511 - INFO # Command              Description
2014-05-06 13:14:38,515 - INFO # ---------------------------------------
2014-05-06 13:14:38,518 - INFO # reboot               Reboot the node
2014-05-06 13:14:38,522 - INFO # id                   Gets or sets the node's id.
2014-05-06 13:14:38,529 - INFO # heap                 Shows the heap state for the LPC2387 on the command shell.
2014-05-06 13:14:38,535 - INFO # ps                   Prints information about running threads.
2014-05-06 13:14:38,544 - INFO # temp                 Prints measured temperature.
2014-05-06 13:14:38,548 - INFO # hum                  Prints measured humidity.
2014-05-06 13:14:38,553 - INFO # weather              Prints measured humidity and temperature.
2014-05-06 13:14:38,557 - INFO # offset               Set temperature offset.
2014-05-06 13:14:38,598 - INFO # dread_sec            Reads the specified sector of inserted memory card
2014-05-06 13:14:38,605 - INFO # dread                Reads the specified bytes from inserted memory card
2014-05-06 13:14:38,610 - INFO # dget_ssize           Get the sector size of inserted memory card
2014-05-06 13:14:38,616 - INFO # dget_scount          Get the sector count of inserted memory card
2014-05-06 13:14:38,622 - INFO # dget_bsize           Get the block size of inserted memory card
2014-05-06 13:14:38,625 - INFO # mersenne_init        initializes the PRNG
2014-05-06 13:14:38,630 - INFO # mersenne_get         returns 32 bit of pseudo randomness
2015-03-09 21:09:52,124 - INFO # rtc                  control RTC peripheral interface.
```

Running the `ps` command on an msba2:

```
2014-05-09 17:38:33,388 - INFO # > ps
2014-05-09 17:38:33,394 - INFO #    pid | name                 | state    Q | pri | stack ( used) location
2014-05-09 17:38:33,401 - INFO #      0 | idle                 | pending  Q |  31 |   160 (  148) 0x40000014
2014-05-09 17:38:33,407 - INFO #      1 | main                 | running  Q |  15 |  2560 (  848) 0x400000b4
2014-05-09 17:38:33,431 - INFO #        | SUM                  |            |     |  4256
```

RIOT specific
=============

The `id` command sets or gets the node's id. It can be used to
identify a node. Boards that support the `config` module will write
the id to a persistent memory location so the node keeps it across
reboots.

The `ps` command is used to analyze the thread's state and memory
status.


Networking
==========

(networking is being reworked, stay tuned!)
