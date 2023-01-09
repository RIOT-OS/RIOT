/**
@defgroup    boards_e104-bt5010a-tb E104-BT5010A Test Board
@ingroup     boards
@brief       Support for the Ebyte E104-BT5010A Test Board (nRF52810)

## General information

The E104-BT5010A Test Board is a cheap break-out board for the E104-BT5010A module.
It's based on the nRF52810 and by default comes with a Firmware that accepts AT
commands over UART, but it can be flashed with RIOT easily.

 - [Module Datasheet](http://www.ebyte.com/en/downpdf.aspx?id=832)
 - [Module Website](http://www.ebyte.com/en/product-view-news.aspx?id=832)

## Flashing the board

To flash the board, you have to connect a SWD programmer to the pins labeled SWDIO and
SWCLK. Also make sure to connect GND.
P0.21 is Reset.

## Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyUSB0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=e104-bt5010a-tb -C examples/hello-world term
```

 */
