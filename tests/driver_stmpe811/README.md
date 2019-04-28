## About

This is the test application for the STMPE811 touchscreen controller.

## Usage

The application works out of the box on a STM32F429I-DISC1 board:

```
make -C tests/driver_stmpe811 flash term
```

## Expected output

The application initializes the STMPE811 and displays "Pressed!" when a touch
event is detected.
Current touch positions are printed in terminal while holding the screen
pressed.
"Released" is displayed when the screen is released.

```
2020-02-10 10:20:17,647 # Pressed!
2020-02-10 10:20:17,653 # X: 167, Y:81
2020-02-10 10:20:17,659 # X: 165, Y:75
2020-02-10 10:20:17,671 # X: 165, Y:69
2020-02-10 10:20:17,689 # X: 166, Y:68
2020-02-10 10:20:17,701 # X: 167, Y:68
2020-02-10 10:20:17,713 # X: 169, Y:69
2020-02-10 10:20:17,725 # X: 170, Y:69
2020-02-10 10:20:17,737 # X: 170, Y:70
2020-02-10 10:20:17,755 # X: 173, Y:69
2020-02-10 10:20:17,761 # Released!
```
