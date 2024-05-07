# About

This is a manual test application for gesture recognition for touch devices
that are using the generic touch device API.

# Usage

This test application initializes the touch device and then waits for touch events using interrupts by default. Once touch events are received, it calls the gesture recognition. The application generates an output like the following:
```
Single Tap X: 276, Y:185
Single Tap X: 271, Y:178
Double Tap X: 271, Y:182
Pressed    X: 235, Y:168
Moving     X: 246, Y:170
Moving     X: 255, Y:171
Moving     X: 266, Y:173
Moving     X: 277, Y:175
Moving     X: 283, Y:176
Moving     X: 294, Y:178
Moving     X: 303, Y:180
Released   X: 310, Y:180
Swipe right
Swipe up
Swipe down
Swipe left
```

To use the touch device in polling mode, the environment variable
`TOUCH_DEV_POLLING_MODE` must be set to 1. The polling period in milliseconds
is defined by the environment variable `TOUCH_DEV_POLLING_PERIOD`. It is
50 ms by default and can be changed by setting the environment variable
`TOUCH_DEV_POLLING_PERIOD` in the make command, for example:
```
TOUCH_DEV_POLLING_MODE=1 TOUCH_DEV_POLLING_PERIOD=100 \
BOARD=... make -C tests/drivers/touch_dev_gestures flash term
```
