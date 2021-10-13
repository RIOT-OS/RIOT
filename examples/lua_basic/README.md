### About

This example shows how to write IoT applications using Lua.

### Caveats

Currently, the only actual function available is "print"

### How to use

Put your lua code into "main.lua" (check the example). The file will
be included in your application as an ASCII byte array. This is done in the
last 20 or so lines of the example's Makefile.

The script will then be run immediately after RIOT has started up.

### How to run

Type `make flash term`

Note: you may have to press `RESET` on the board (after the flash) if the board
reboots faster than the terminal program can start..
