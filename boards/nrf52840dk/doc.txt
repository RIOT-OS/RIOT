/**
@defgroup    boards_nrf52840dk nRF52840 DK
@ingroup     boards
@brief       Support for the nRF52840 DK


### RESET pin configuration

On many (all?) nrf52840dk boards, the reset pin is not configured out-of-the box.
This means, that simply nothing happens if the RESET pin is pressed. To change
this, RIOT provides a little tool in `dist/tools/nrf52_resetpin_cfg`.

Simply compile, flash, and run that tool on your board, and the reset pin should
work for the time being.

## Flash the board

See the `Flashing` section in @ref boards_common_nrf52.

## Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=nrf52840dk -C examples/hello-world term
```

 */
