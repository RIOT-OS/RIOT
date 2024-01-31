## About

This is the test application for the STMPE811 touchscreen controller.

## Usage

The application works out of the box on a STM32F429I-DISC1 board using the
interrupt mode by default:

```
make -C tests/drivers/stmpe811 flash term
```

To use the polling mode, the environment variable `STMPE811_POLLING_MODE` must
be set to 1. The polling period in milliseconds is defined by the environment
variable `STMPE811_POLLING_PERIOD`. It is 50 ms by default. It can be changed
by setting the environment variable `STMPE811_POLLING_PERIOD` in the make
command, for example:
```
STMPE811_POLLING_MODE=1 STMPE811_POLLING_PERIOD=100 BOARD=... make -C tests/drivers/touch_dev flash term
```

## Expected output

The application initializes the STMPE811 and displays "Pressed!" when a touch
event is detected. The position of the touch event is also displayed.
"Released" is displayed when the screen is released.

```
2020-07-21 21:24:49,286 # Pressed!
2020-07-21 21:24:49,293 # X: 132, Y:138
2020-07-21 21:24:49,826 # Released!
2020-07-21 21:24:51,218 # Pressed!
2020-07-21 21:24:51,219 # X: 42, Y:16
2020-07-21 21:24:51,614 # Released!
2020-07-21 21:24:53,385 # Pressed!
2020-07-21 21:24:53,385 # X: 197, Y:64
2020-07-21 21:24:53,588 # Released!
```
