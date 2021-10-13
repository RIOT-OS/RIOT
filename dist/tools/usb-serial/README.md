USB to serial adapter tools
================================

Tools for finding connected USB to serial adapter devices.

Usage
-----

    ./list-ttys.sh

List all currently connected USB to serial adapters by searching through
`/sys/bus/usb/devices/`.

    ./find-tty.sh [serial_regex1] [serial_regex2] ... [serial_regexZ]

Write to `stdout` all ttys connected to the chosen programmer.
`serial_regexN` are extended regular expressions (as understood by `egrep`)
containing a pattern matched against the USB device serial number. Each of the
given expressions are tested, against each serial number, and matching ttys are
output (one tty per line).

In order to search for an exact match against the device serial, use
'^serialnumber$' as the pattern. If no pattern is given, `find-tty.sh` returns
all found USB ttys (in an arbitrary order, this is not guaranteed to be
the `/dev/ttyUSBX` with the lowest number).

Serial strings from all connected USB ttys can be found from the list generated
by `list-ttys.sh`.

Exit codes
----------
`find-tty.sh` returns 0 if a match is found, 1 otherwise.

Makefile example usage
----------------------

The script `find-tty.sh` is designed for use from within a board
`Makefile.include`. An example section is shown below (for an OpenOCD based
solution):

    # Add serial matching command
    ifneq ($(PROGRAMMER_SERIAL),)
      OOCD_BOARD_FLAGS += -c 'ftdi_serial $(PROGRAMMER_SERIAL)'
    endif

    PORT_LINUX_EXACT = $(if $(PROGRAMMER_SERIAL),$(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh "^$(PROGRAMMER_SERIAL)$$")),)

    PORT_LINUX = $(if $(PORT_LINUX_EXACT),$(PORT_LINUX_EXACT),$(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh)))

    PORT_DARWIN = $(shell ls -1 /dev/tty.SLAB_USBtoUART* | head -n 1)



Limitations
-----------

Only tested on Linux, and probably only works on Linux.
