# Description

This test verifies that the uMorse package compiles with RIOT. It also checks
its basic functions by encoding a text string into Morse code. The encoded text
is printed to the shell using `.` and `_` for DIT (short) and  DAH (long) Morse
chars. Inter char gaps are represented by ` ` (space), inter word gaps are
printed as ` / ` (dash). Even longer gaps are printed as `\n` (new line).

The Morse code will be printed 2 times using different encoding strategies
provided by uMorse. The first run is printed with delays, i.e., the Morse
code is printed with a certain timing. The second run is printed without
delay, i.e., Morse code is printed all at once. However, if compiled for a
board with a LED0_ON defined, the second run will use the LED0 to blink the
Morse encoded text with delays.

# Configure delays

The delay can be configured by setting `UMORSE_DELAY_DIT`, see Makefile.
The default in uMorse is 60ms, as this is quite fast for beginners, its
set to 120ms for this test and could be further raised, e.g. 240ms.
