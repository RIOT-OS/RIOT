# About

This is a manual test application for testing RN2483/RN2903 LoRa Microchip
modules:
* [RN2483](http://ww1.microchip.com/downloads/en/DeviceDoc/50002346C.pdf)
* [RN2903](http://ww1.microchip.com/downloads/en/DeviceDoc/50002390C.pdf)

RN2483 can be used in Europe region and RN2903 in US region.

For easy testing, those modules can be found soldered on Bee-like layout. To
run this test with this kind of setup, you need to connect the following pins
of the Bee-like module to your board:
- UART RX
- UART TX
- VCC (3V3)
- GND

NOTE: when you use an Arduino Wireless Proto shield, the RN2483/RN2903 module
can potentially be connected to the same UART as RIOTs standard out. This is the
case for Arduino UNO, and most of the Nucleo 64 boards.
In this case you must redefine `STDIO_UART_DEV` to another UART interface in
the `board.h` and connect a UART-to-USB adapter to that UART.
This is not the case with Arduino-zero or some Nucleo144 boards.

# Build and flash the application

Use the `DRIVER` variable to select the right driver module between `rn2483`
and `rn2903` depending on your region.
Example in EU:
```
    make DRIVER=rn2483 BOARD=arduino-zero -C tests/driver_rn2xx3 flash term
```

# Usage

For testing the LoRaBee driver, use the shell commands provided by this
test application: `sys` and `mac`:
* `sys` contains subcommands for controlling the device at a low-level
* `mac` contains subcommands for connecting to a LoRaWAN network, sending,
  receiving paquets and getting/setting some LoRaMAC parameters.

The full list of available parameters for each command is described in the
following documents:
- For EU region (868MHz and 433MHZ bands):
[RN2483](http://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf)
- For US region (915MHZ band):
[RN2903](http://ww1.microchip.com/downloads/en/DeviceDoc/40001811A.pdf)

The radio level commands are not supported by this driver (yet).

Examples of commands:
* Reset the module:
```
    > sys reset
    RN2483 1.0.1 Dec 15 2015 09:38:09
```
* Put the module into sleep (5s is the default):
```
    > sys sleep
```
* Set a LoRaMAC dev eui (put yours instead of the fake one given as
  example here), 8 bytes in hex string representation:
```
    > mac set deveui 0000000000000000
```
* Set a LoRaMAC application eui, 8 bytes in hex string representation:
```
    > mac set appeui 0000000000000000
```
* Set a LoRaMAC application key, 16 bytes in hex string representation:
```
    > mac set appkey 00000000000000000000000000000000
```
* Save your parameters in the module EEPROM. After that, setting deveui,
  appeui, appkey is not required anymore, even after rebooting the module:
```
    > mac save
```
Warning: when using **OTAA** activation, it's important to not use `mac save`
**after joining the network**. Thus, **before starting the join procedure**,
ensure a null devaddr is written to the EEPROM:
```
    > mac set devaddr 00000000
    > mac save
```
* Join your network using Other-the-air-activation:
```
    > mac join otaa
```
* Send data `AAAA` to the LoRaWAN server:
```
    > mac tx AAAA
```
