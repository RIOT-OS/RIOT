# Runtime Config Core

This application demonstrates the most basic usage of the Runtime config.
It implements a BOARD LED schema and continuously changes its value every second.

## Usage

Simply build and flash the application for your target board:

```shell
BOARD=YOUR_BOARD_NAME_HERE make flash term
```

Now you should see the terminal continuously print out different LED states.
