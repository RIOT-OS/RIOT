The purpose of this test program is to verify that the `arduino_sdi_12` package
works.

To test it you will need a SDI-12 sensor. The SDI-12 protocol uses 3 wires:
power, ground, and data. Use the D13 pin for the data line.

The program will loop forever. In every loop it:

- Tries to find the address of the attached sensor (sends the `?!` command);
- Asks the sensor to identify itself (sends the `aI!` command);
- Asks the sensor to start a measurement (sends the `aM!` command);
- Waits for as many seconds as the sensor said in its response to the
  measurement command;
- Asks the sensor for the data, the result of the measurement (sends the `aD0!`
  command);
- Waits 5 seconds and starts the loop again.

This is an example, with the Arduino Mega2560 board and the Decagon CTD-10
sensor:

    $ BOARD=arduino-mega2560 make -C tests/pkg_arduino_sdi_12 all flash term
    [...]
    2021-09-28 12:14:57,492 # main(): This is RIOT! (Version: 2021.10-devel-776-gc7af21-sdi12)
    2021-09-28 12:14:57,496 # Testing the Arduino-SDI-12 package
    2021-09-28 12:14:57,995 #
    2021-09-28 12:14:58,007 # Send: ?!
    2021-09-28 12:14:58,352 # Recv: 0
    2021-09-28 12:14:58,364 # Send: 0I!
    2021-09-28 12:14:58,749 # Recv: 013DECAGON CTD-103991059303507
    2021-09-28 12:14:58,761 # Send: 0M!
    2021-09-28 12:14:59,118 # Recv: 00013
    2021-09-28 12:15:00,129 # Send: 0D0!
    2021-09-28 12:15:00,502 # Recv: 0+35+26.4+0
    2021-09-28 12:15:05,507 #
    2021-09-28 12:15:05,519 # Send: ?!
    2021-09-28 12:15:05,863 # Recv: 0
    2021-09-28 12:15:05,875 # Send: 0I!
    2021-09-28 12:15:06,260 # Recv: 013DECAGON CTD-103991059303507
    2021-09-28 12:15:06,268 # Send: 0M!
    2021-09-28 12:15:06,629 # Recv: 00013
    2021-09-28 12:15:07,641 # Send: 0D0!
    2021-09-28 12:15:08,013 # Recv: 0+36+26.4+0
