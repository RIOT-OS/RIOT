@defgroup   boards_thingy52 Nordic Thingy:52
@ingroup    boards
@brief      Support for the Nordic Thingy:52 board

## Overview
The Nordic Thingy:52 is a battery powered platform with Bluetooth and NFC (only tag emulation, no reader support)
radio, a wide range of sensors and some output capabilities.

Components:

- nRF52832 main controller (providing Bluetooth and NFC tag emulation)
- an LIS2DH12 low-power three-axis linear accelerometer
- an MPU-9250 nine-axis motion sensor
- an LPS22HB air pressure and temperature sensor
- an HTS221 air temperature and humidity sensor
- a BH1745NUC color sensor
- a CCS811 gas sensor
- an MP34DB02 digital microphone
- a speaker
- two RGB LED channels: one LED pair in the light well, and one RGB LED to assist the light sensor
- a button
- an XC6804A4E1 battery charge controller (providing charge error indication)
- USB power supply and battery voltage sensing
- four DMN3190LDW MOSFETs

Note that at the moment, only the button and a single UART (at P6) are supported in RIOT.

## Links

- [Device description](https://infocenter.nordicsemi.com/topic/ug_thingy52/UG/thingy52/intro/frontpage.html)

## Flash the board

The board can be flashed using an external programmer on its 10-pin debug port P9.

See the `Flashing` section in @ref boards_common_nrf52.

Its over-the-air bootloading mechanism is currently not supported in RIOT.
