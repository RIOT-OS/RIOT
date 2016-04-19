Expected result
===============
This test application is polling all available SAUL devices periodically for
their data. This data is then printed to STDIO.

Background
==========
The actual devices that are actually polled depend on the devices that are
configured for a particular platform. If you want to test the SAUL interface
to a particular device, you should just connect it to your BOARD and include
the corresponding device driver using the `USEMODULE` environment variable.
