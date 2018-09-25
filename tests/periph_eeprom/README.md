Expected result
===============

Use the provided shell commands to read and write bytes from/to the MCU's
internal EEPROM memory.

    # Read 10 bytes from the beginning of the eeprom
    > read 0 10

    # Write HelloWorld starting from the 10th position in the eeprom
    > write 10 HelloWorld

    # Test EEPROM access speed (TEST_BUFFER_SIZE bytes starting from 0)
    > test

Background
==========

This test application provides shell commands to verify the implementations of
the `eeprom` peripheral driver interface.
