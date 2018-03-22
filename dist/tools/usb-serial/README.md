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

      ifeq ($(PORT),)
        # try to find tty name by serial number, only works on Linux currently.
        ifeq ($(OS),Linux)
          PORT := $(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh "^$(PROGRAMMER_SERIAL)$$"))
        endif
      endif
    endif

    # Fallback PORT if no serial was specified or if the specified serial was not found
    ifeq ($(PORT),)
        ifeq ($(OS),Linux)
          PORT := $(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh))
        else ifeq ($(OS),Darwin)
          PORT := $(shell ls -1 /dev/tty.SLAB_USBtoUART* | head -n 1)
        endif
    endif

    # TODO: add support for windows as host platform
    ifeq ($(PORT),)
      $(info CAUTION: No terminal port for your host system found!)
    endif
    export PORT


Limitations
-----------

Only tested on Linux, and probably only works on Linux.
