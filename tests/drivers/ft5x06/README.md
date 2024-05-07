# About

This is a manual test application for the FT5x06 touch device driver.

# Usage

The test application initializes the FT5x06 touch device and then waits
for touch events by using interrupts by default. When touch events occur,
the application generates output like the following:
```
+------------Initializing------------+
Initialization successful
1 touch detected
Touch 1 - X: 203, Y:156
Touch 1 - X: 204, Y:157
Touch 1 - X: 204, Y:157
Touch 1 - X: 206, Y:158
Touch 1 - X: 210, Y:159
Touch 1 - X: 210, Y:159
Touch 1 - X: 218, Y:160
Released!
```

To use the touch device in polling mode, the environment variable
`FT5X06_POLLING_MODE` must be set to 1. The polling period in milliseconds
is defined by the environment variable `FT5X06_POLLING_PERIOD`. It is
50 ms by default. It can be changed by setting the environment variable
`FT5X06_POLLING_PERIOD` in the make command, for example:
```
FT5X06_POLLING_MODE=1 FT5X06_POLLING_PERIOD=100 BOARD=... make -C tests/drivers/touch_dev flash term
```
