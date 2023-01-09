/**
@defgroup    boards_nrf52840-mdk nRF52840-MDK
@ingroup     boards
@brief       Support for the nRF52840-MDK

### General information

The Makerdiary [nRF52840-MDK](https://github.com/makerdiary/nrf52840-mdk) board
is an opensource, micro development kit using the nRF52840 SoC.
This board provides 802.15.4 and BLE connectivity.

### Pinout

![nRF52840-MDK](https://github.com/makerdiary/nrf52840-mdk/blob/master/docs/images/nrf52840-mdk-pinout.jpg?raw=true)

### Flash the board

By default, the board is flashed with PyOCD programmer via a DAPLink.

PyOCD can be installed using Python package manager:
```
    pip install pyocd --user -U
```

See the `Flashing` section in @ref boards_common_nrf52.


### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=nrf52840-mdk -C examples/hello-world term
```
 */
