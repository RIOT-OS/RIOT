examples/saul
================
This application demonstrates the usage of SAUL and the SAUL registry.

Usage
=====
Simply build and flash the application for your target board:
```
BOARD=YOUR_BOARD_NAME_HERE make flash term
```

Now you should have access to the RIOT shell on your board. For interacting
with SAUL devices, use the `saul` shell command, e.g.:
```
saul            <- this will list all available devices mapped into
                   the SAUL registry
saul read 0     <- read values from device #0
saul write 1 0  <- write a `0` to SAUL device #1
```
