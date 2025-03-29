@defgroup    boards_seeedstudio-xiao-nrf52840-sense Seeed Studio XIAO nRF52840 Sense
@ingroup     boards
@brief       Support for the Seeed Studio XIAO nRF52840 Sense

### General information

The [XIAO nRF52840 Sense][seeedstudio-xiao-nrf52840-sense]
is a development board from Seeed Studio's XIAO board family.

It provides native USB support, Bluetooth
Low Energy and IEEE 802.15.4 support via the nRF52840 MCU.
Compared to the [XIAO nRF52840](@ref boards_seeedstudio-xiao-nrf52840) it
features an ST LSM6DS3TR-C IMU and a PDM Microphone (not supported yet).

<img src="https://files.seeedstudio.com/wiki/XIAO-BLE/pinout2.png"
     alt="top-down view on seeedstudio-xiao-nrf52840" width="50%"/>

[seeedstudio-xiao-nrf52840-sense]: https://wiki.seeedstudio.com/XIAO_BLE/

### Using the LSM6DS3TR-C IMU

You can test the built-in IMU with the test provided in `tests/drivers/lsm6dsxx`:

```shell
make BOARD=seeedstudio-xiao-nrf52840-sense -C tests/drivers/lsm6dsxx flash term
```

### Flashing, Bootloader, and Terminal

Refer to the [Adafruit nRF52 Bootloader](@ref boards_common_adafruit-nrf52-bootloader)
documentation for further details on the flashing process.

Example with `hello-world` application:
```shell
make BOARD=seeedstudio-xiao-nrf52840-sense -C examples/basic/hello-world flash term
```
