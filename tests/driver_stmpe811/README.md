## About

This is the test application for the STMPE811 touchscreen controller.

## Usage

The application works out of the box on a STM32F429I-DISC1 board:

```
make -C tests/driver_stmpe811 flash term
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
