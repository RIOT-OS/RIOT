Expected result
===============

This test application attempts to initialize some configured sens0 devices,
followed by polling all available SAUL devices periodically for their data.
This data is then printed to STDIO.

Background
==========

The actual devices that are actually polled depend on the devices that are
configured for a particular platform.
The main purpose of the application is to ensure that sens0 is compile tested by
the CI system, as well as giving a basic example of how to configure and
initialize sens0 devices.
