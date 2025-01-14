### About

This example shows how to write IoT applications using javascript.

### Acknowledgement
This example is based on [Jerryscript](https://github.com/jerryscript-project/jerryscript) which does all the heavy lifting, providing full ECMAScript 5.1 profile on your IoT device (kudos guys!).

### Caveats

- currently, the only actual function available is "print"

### How to use

Just put your javascript code in "main.js" (check the example). The file will
automatically be included when compiling the application, which will execute
the file right after booting.

### How to run

Type `make flash term`.

Note: you may have to press `RESET` on the board (after the flash) if the board
reboots faster than the terminal program can start..
