# About
This is a test application for the soft 1-Wire driver. It will scan each soft
1-Wire bus for attached devices, and then print their IDs to the console.

# Configuring The Bus
If your board already defines one or more 1-Wire buses, those will be used, and
no configuration should be necessary. Otherwise, edit `./Makefile` to define the
pin to use as a 1-Wire bus and override any of the driver defaults as necessary.
