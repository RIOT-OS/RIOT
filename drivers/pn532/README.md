Pn532 Driver NFC
================
This library is for pn532's chips via SPI communication.
Using it for your udoo board, you should be able to interactively use any
Adafruit PN532 Shield right connected to ICSP header and CS on pin 23.
It should also works with other boards wired with this device.

Usage
=====

Build, flash and start the application:
```
export BOARD=udoo
make clean all
make flash
```

RIOT specific
=============
It's important to choose a frequency from 400 KHz to 5 MHz maximum.
When it's flashed Auth_Mifare program, NFC reader needs some seconds
to authenticate with the tag.
It's reccomended to don't change any configurations if you don't know
any purpose.


