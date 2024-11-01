@defgroup    boards_seeedstudio-xiao-nrf52840 Seeed Studio XIAO nRF52840
@ingroup     boards
@brief       Support for the Seeed Studio XIAO nRF52840

### General information

The [XIAO nRF52840][seeedstudio-xiao-nrf52840] (formally known as XIAO BLE)
is a development board from Seeed Studio's XIAO board family.

It provides native USB support, Bluetooth
Low Energy and IEEE 802.15.4 support via the nRF52840 MCU.

<img src="https://files.seeedstudio.com/wiki/XIAO-BLE/pinout2.png"
     alt="top-down view on seeedstudio-xiao-nrf52840" width="50%"/>

[seeedstudio-xiao-nrf52840]: https://wiki.seeedstudio.com/XIAO_BLE/

### Flashing, Bootloader, and Terminal

Refer to the [Feather nRF52840 Express
documentation](https://doc.riot-os.org/group__boards__feather-nrf52840.html) for further details.
Both use the same flasher, bootloader, and terminal settings.

Example with `hello-world` application:
```shell
make BOARD=seeedstudio-xiao-nrf52840 -C examples/basic/hello-world flash term
```
