Expected result
===============
This test enables you to test all available low-level I2S functions using the shell.
Consult the `help` shell command for available actions.

Background
==========
Test for the low-level I2S driver.

Glossary
==========
DEV - The I2S device number, this is usually 0.
Consult the board header file for more information.</br>
MODE - 'controller' or 'target', configures which side controls the clock lines.</br>
DIR - Direction of the data flow, one of 'rx', 'tx' or 'both'. Depends on peripheral support.</br>
CHANNELS - Number of channels in the audio stream, usually 1 or 2.</br>
CLOCK_HZ - Frequency of the frame clock. Usually 48000.</br>
BUF_BYTES - Number of bytes in the buffer used per transaction, between 8 and 128 with the default config.</br>
WORD_BITS - Number of bits in a single audio sample. Usually 16, 24 or 32.</br>
