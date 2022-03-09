This utility downloads code to Philip LPC 2000 series chips, using the
bootloader communication protocol documented in the LPC2106/2105/2104
User Manual, Sept 2003 revision, pages 177 to 192.  This code has also
been tested with LPC2131 and LPC2138 chips (thanks to New Micros for
providing eval boards).  It may work with other Philips ARM LPC parts.



Usage:
-----

When you start the program, it's small control window appears, and a
xterm terminal window is launched.  The Xterm window allows you to
simply leave lpc2k_pgm running and interface with the LPC uart using
that window.  Most people configure "printf" on in their code to print
to UART0, which will appear in this window.

This program has 4 settings:

Firmware:  The intel-hex file with your LPC firmware to program.
Port:      Which serial device to use.
Baud:      The baud rate to communicate.
Crystal:   The speed of the crystal on your LPC board.

Once you have set up these values, simply press the "Program Now"
button to write your firmware to the LPC flash memory, and automatically
run it.  If your firmware communicates on UART0, its messages will appear
in the xterm window, and anything you type in that window will be
transmitted to your board.

The "Reboot" button may be used to reboot your code (assuming you have
connected DTR appropriately).  The "Bootloader" button may be used to
stop your code by rebooting into the bootloader, rather than your program.



Hardware Requirements:
---------------------

You must have a compatible Philips LPC chip with its UART0 interfaced
to a serial port on your PC.

You must be able to reset the chip and cause it to enter bootloader mode.
Normally, this is done by connecting the (TTL level translated) DTR signal
to the LPC reset, so that when DTR is high (the TTL version is low), the
Philips chip is in reset mode.  Alternately, a pushbutton may be used,
but you will need to manually press the button every time you want to get
back into bootloader mode (while this program attempts to sync baud rates),
rather than letting DTR do it automatically.  A few minutes spent wiring
up your circuit so DTR can reset the board will save you much trouble as
you develop code.

P0.14 must be connected low shortly after reset.  Normally, this pin
is just shorted to ground using a jumper.  Starting with version 1.02,
you can simply leave this shorted to ground.  If your design needs to
use this pin, you may also build a simple circuit that forces this pin
to ground when RTS is high.



Software Requirements:
---------------------

You must have a Linux-based system running X Windows.  This code has
been tested with Linux kernel 2.4.20 and 2.6.8, and should work with
almost any linux system.

You must have the "xterm" program installed.  Nearly all linux
distrubtions provide this, and it is often installed by default.  If
you do not have it, simply install from your linux distribution.

Your serial port device file (usually /dev/ttyS0 or /dev/ttyS1) must
allow permission for you to use the serial port.

GTK 1.2 is used for the GUI.  Many newer systems only have GTK version
2 (or higher).  If you have one of these systems, perhaps you can
install GTK 1.2 (including the development libraries) to allow you to
compile this code.  Alternately, you may be able to use the semi-static
build, which includes a copy of this code built into the program.



Building and Installation:
-------------------------

This software is only provided semi-static binary and source code form.

To use the semi-static binary, simply copy it to a location where you
can run it, and change the permissions if necessary:

    cp lpc2k_pgm /usr/local/bin
    chmod 755 /usr/local/bin/lpc2k_pgm

The semi-static binary has all of the GTK and X11 libraries statically
linked into it, for maximum compatibility with all linux distributions.
The only disadvantage is, of course, that this uses an extra 1.4 megs
of RAM, with (might) otherwise be shared with other programs.  If you
simply want to run this program with minimal work, using the semi-static
binary may be the easiest way.

If you compile from the source code, the result should be a small
binary that is optimal for your system.

To build from source, you must have GTK+ 1.2 development libraries
and GCC properly installed.  Nearly all linux distributions provide
these as packages, but you may need to install them before you can
compile the code.

TODO: specific instructions for Debian stable
TODO: specific instructions for Debian testing/unstable
TODO: specific instructions for Fedora
TODO: specific instructions for Suse

Simply type "make" to build the code.  The resulting "lpc2k_pgm"
program can be run from any location.  Simply copy to /usr/local/bin,
or where ever you like.



Contact Info:
------------

Paul Stoffregen
paul@pjrc.com
http://www.pjrc.com/arm/lpc2k_pgm

If you discover a bug, you want to request a new feature, or you have
a new Philips LPC chip which is not recognized, please attempt to
provide COMPLETE information in your message.

If you have problems building from source, please contact me with ALL
of the following:

1: Complete copy of all messages during the build.
2: Output of "gtk-config --version"
3: Output of "gtk-config --libs"
4: Output of "gtk-config --cflags"
5: Output of "uname -a"
6: Other info... which linux distribution, version, other software

If you get "Command not found" when trying to run "gtk-config", this
is a sure sign that you do not have GTK+ 1.2 installed.
