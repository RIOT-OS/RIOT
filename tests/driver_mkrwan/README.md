# About

This is a manual test application for testing the Arduino MKRWAN 1300 on-board
LoRa radio.

# Build and flash the application

    make BOARD=arduino-mkrwan1300 -C tests/driver_mkrwan flash

# Usage

This test application provides a shell with basic commands to interact with
the radio.

Note: the Device EUI of the LoRa cannot be changed.

* Read current version of the LoRa module:

      > mkrwan version


* Read the device EUI (invalid value given on purpose):

      > mkrwan get deveui
      device eui: 0000000000000000

For an OTAA activation procedure:

* Set the application EUI (invalid value given on purpose):

      > mkrwan set appeui 0000000000000000

* Set the application key:

      > mkrwan set appkey 00000000000000000000000000000000

* Join the network:

      > mkrwan join otaa
      Join procedure succeeded!

* Send some data to the LoRaWAN network:

      > mkrwan send ThisIsRIOT!
      Data sent with success

You can also play with LoRaWAN basic parameters:

* Change the datarate to 5 (SF7BW125):

      > mkrwan set dr 5

* Turn off adaptive datarate:

      > mkrwan set adr off
