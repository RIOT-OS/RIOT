# About
This is a manual test application for the PIR motion sensor driver.

In order to build this application, you need to add the board to the
Makefile's `WHITELIST` first and define a pin mapping (see below).


# Usage
There are two ways to test this. You can either actively poll the sensor
state, or you can register a thread which receives messages for state
changes.

## Interrupt driven
Connect the sensor's "out" pin to a GPIO of your board that can be
configured to create interrupts.
Compile and flash this test application like:

    export BOARD=your_board
    export PIR_GPIO=name_of_your_pin
    make clean
    make all-interrupt
    make flash

The output should look like:

    kernel_init(): jumping into first task...

    PIR motion sensor test application

    Initializing PIR sensor at GPIO_8... [OK]

    Registering PIR handler thread...     [OK]
    PIR handler got a message: the movement has ceased.
    PIR handler got a message: something started moving.
    PIR handler got a message: the movement has ceased.


## Polling Mode
Connect the sensor's "out" pin to any GPIO pin of you board.
Compile and flash this test application like:

    export BOARD=your_board
    export PIR_GPIO=name_of_your_pin
    make clean
    make all-polling
    make flash

The output should look like this:

    kernel_init(): jumping into first task...
    PIR motion sensor test application

    Initializing PIR sensor at GPIO_10... [OK]

    Printing sensor state every second.
    Status: lo
    ...
    Status: lo
    Status: hi
    ...
