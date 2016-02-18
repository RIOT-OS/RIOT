Expected result
===============
The 'red' and the 'green' LEDs of the board should light up alternating with a
500ms interval. If your board has only one LED (probably defined as LED_RED),
you will see only this LED blink at 1Hz. If your board has no on-board LED, you
will see nothing.

Background
==========
All boards in RIOT define at least two macros for accessing two selected
on-board LEDs directly. These are called LED_RED and LED_GREEN, independent if
the actual color of those LED is red or green.
