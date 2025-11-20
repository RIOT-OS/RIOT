examples/arduino_hello-world
============================
This application demonstrates the usage of Arduino sketches in RIOT.

The sketch itself is fairly simple. On startup, it initializes the LED pin to
output mode, starts the serial port with a baudrate of 115200 and prints
"Hello Arduino!" on the serial port. When running, the application echoes any
newline terminated string that was received on the serial port, while toggling
the default LED with a 1Hz frequency.

The sketch just uses some very primitive Arduino API elements for demonstration
purposes:
- control of digital pins (pinMode(), digital read and write)
- the delay() function
- reading and writing the serial port using the Serial class

Arduino and RIOT
================
For information on the Arduino support in RIOT please refer to the API
documentation at http://doc.riot-os.org/group__sys__arduino.html

Usage
=====
Just send any newline terminated string to the board's serial port, and the
board will echo this string.

Example output
==============
When using pyterm, the output will look similar to this:
```
2015-11-26 14:04:58,307 - INFO # main(): This is RIOT! (Version: xxx)
2015-11-26 14:04:58,308 - INFO # Hello Arduino!
hello again
2015-11-26 14:06:29,800 - INFO # Echo: hello again
are you still there?
2015-11-26 14:06:48,301 - INFO # Echo: are you still there?

```
If your board is equipped with an on-board LED, you should see this LED toggling
every half a second.

NOTE: if your board's STDIO baudrate is not configured to be 115200 (see your
board's `board.h`), the first line of the output may not be shown or scrambled.
