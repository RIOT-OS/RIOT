tests/driver_srf04
================
This example shows the usage of an srf04 module.
The application uses a timer and two gpio pins.
The module is an ultrasonic range finder with a small protocol [1]:
- trigger pin is raised high for 10 us by calling trigger function
- after a sample period of 50 ms the value can be read out

[1] www.robot-electronics.co.uk/htm/srf04tech.htm

Usage
=====

Build, flash and start the application:

    export BOARD=your_board
    make flash term
