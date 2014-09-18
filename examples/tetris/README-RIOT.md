# RIOT Tetris

This is a quick and dirty port of micro tetris to RIOT.
It has only been tested on samr21-xpro.

You need a terminal which passes all key presses directly, without waiting for
a full line.  I use picocom:

    sudo picocom -b 115200 /dev/ttyACM3 --imap lfcrlf

Have fun!

Kaspar

# known issues

- the native uart somehow doesn't work
- to turn off echo and break for native, try "stty cbreak -echo ; path-to/tetris.elf ; stty sane"
