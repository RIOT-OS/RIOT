# About

This is a manual test application for touch device drivers using the generic
touch device API. Which touch device driver is used is determined by the board
definition.

# Usage

This test application initializes the touch device and then waits
for touch events by using interrupts by default. When touch events occur,
the application generates output like the following:
```
Event: pressed!
X: 157, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
X: 158, Y:152
Event: released!
```

To use the touch device in polling mode, the environment variable
`TOUCH_DEV_POLLING_MODE` must be set to 1. The polling period in milliseconds
is defined by the environment variable `TOUCH_DEV_POLLING_PERIOD`. It is
50 ms by default. It can be changed by setting the environment variable
`TOUCH_DEV_POLLING_PERIOD` in the make command, for example:
```
TOUCH_DEV_POLLING_MODE=1 TOUCH_DEV_POLLING_PERIOD=100 BOARD=... make -C tests/drivers/touch_dev flash term
```
