/**
@defgroup    boards_nrf52832-mdk nRF52832-MDK
@ingroup     boards
@brief       Support for the nRF52832-MDK

### General information

The Makerdiary [nRF52832-MDK](https://github.com/makerdiary/nrf52832-mdk) board
is an opensource, micro development kit using the nRF52832 SoC.
This board provides BLE connectivity.

The nRF52832-MDK v2 version provides a convenient USB dongle form factor.

### Pinout

For nRF52832-MDK v1:

<img src="https://raw.githubusercontent.com/makerdiary/nrf52832-mdk/master/docs/images/nrf52832_mdk_v1_pinout.jpeg"
     alt="pinout" style="height:800px;"/>

For nRF52832-MDK v2:

<img src="https://raw.githubusercontent.com/makerdiary/nrf52832-mdk/master/docs/images/nrf52832_mdk_v2_pinout.jpeg"
     alt="pinout" style="height:800px;"/>

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
    make BOARD=nrf52832-mdk -C examples/hello-world term
```
 */
