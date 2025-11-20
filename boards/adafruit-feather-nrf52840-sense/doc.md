@defgroup    boards_adafruit-feather-nrf52840-sense Adafruit Feather nRF52840 Sense
@ingroup     boards
@brief       Support for the Adafruit Feather nRF52840 Sense

### General information

The [Feather nRF52840 Sense][feather-nrf52840-sense] is a development board
from Adafruits Feather board family. It is very similar to the [Feather
nRF52840 Express][feather-nrf52840] but comes with more sensors.

It provides native USB support, Bluetooth
Low Energy and IEEE 802.15.4 support via the nRF52840 MCU.

<img src="https://cdn-learn.adafruit.com/assets/assets/000/089/096/original/sensors_Feather_Sense_top.jpg"
     alt="top-down view on feather-nrf52840-sense" width="50%"/>

[feather-nrf52840]: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/
[feather-nrf52840-sense]: https://learn.adafruit.com/adafruit-feather-sense

### Flashing, Bootloader, and Terminal

Refer to the [Feather nRF52840 Express](@ref boards_adafruit-feather-nrf52840-express) documentation for further details.
Details to the Flashing process can be found in the common
[Adafruit nRF52 Bootloader](@ref boards_common_adafruit-nrf52-bootloader) documentation.
Both use the same flasher, bootloader, and terminal settings.

Example with `hello-world` application:
```sh
make BOARD=adafruit-feather-nrf52840-sense -C examples/basic/hello-world flash term
```
