Expected result
===============

Use a probe to examine the output from the UART. It tests all
permutations of data-bits, parity, and stop-bits. For each mode, the mode
configuration will be printed to STDOUT as:
<data-bits><parity-bits><stop-bits>
For example: 7 data-bits, even parity, and 2 stop-bits would be: 7E2
Only supported mode strings will be printed out. At the end of the
test, the default mode will be restored, and a list indicating which
modes were supported and unsupported by the device. The scope still
needs to be used to validate that the settings were applied properly.
If a different BAUD rate than 115200 is desired, compile with:
`CFLAGS+=-DSTDIO_UART_BAUDRATE=<BAUD>`

Background
==========

This test was created because the existing periph_uart test relies on the
presence of multiple UARTs, so that one can be used for the shell, and another
for testing. This test requires no shell hookup, and automatically runs through
the test procedure, relying on a probe to examine the results.