examples/wiegand
================
This application is a showcase for using the Wiegand driver.

Wiegand is a protocol widely used in RFID card readers, access keyboards
and other similar devices.

This example simulates a 26bit Wiegand frame and decodes it using the
driver.

For this to work you'll need to use a board that supports flank-triggered
GPIOs inputs.

To make it work, connect together WG_IN_D0 with WG_OUT_D0 and
WG_IN_D1 with WG_OUT_D1. Check the pin designations at the beginning of the
file main.c.

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

On boot it will show an output similar to this one:

```
2019-09-12 18:58:51,826 - INFO # main(): This is RIOT! (Version: compilation)
2019-09-12 18:58:51,827 - INFO # Welcome to RIOT!
2019-09-12 18:58:51,828 - INFO # Starting Wiegand reader thread...
2019-09-12 18:58:51,830 - INFO # Starting Wiegand simulation thread...
2019-09-12 18:58:51,831 - INFO # Sending WG frame...
2019-09-12 18:58:51,832 - INFO # All up, running the shell now
> 2019-09-12 18:58:51,841 - INFO #  WG frame sent...
2019-09-12 18:58:56,846 - INFO # Sending WG frame...
2019-09-12 18:58:56,878 - INFO # WG frame sent...
2019-09-12 18:58:56,925 - INFO # Card found..
2019-09-12 18:58:56,926 - INFO # Card high: 0
2019-09-12 18:58:56,927 - INFO # Card low:  1943

```

It will repeat every 5 seconds.

RIOT specific
=============

Shell commands are available to monitor memory usage.

The `ps` command is used to analyze the thread's state and memory
status.
