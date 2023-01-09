/**
@defgroup    boards_common_nrf52 nRF52 common
@ingroup     boards_common
@brief       Common files and configuration for nRF52 boards.

## Flashing nrf52 boards

By default, nrf52 boards are flashed using Jlink. @ref boards_nrf52840-mdk
uses PyOCD by default.

To flash the board, use `BOARD=<nrf52 board>` with the `make` command.<br/>
Example with `hello-world` application:
```
    make BOARD=<nrf52 board> -C examples/hello-world flash
```

OpenOCD can also be used to flash nrf52 boards (except thingy52 and ruuvitag
because they are hardly coupled with JLink segger_rtt stdio).
For the moment, the latest stable version of OpenOCD (0.10) doesn't contain any
support for nrf52 but versions built against the current development version
can be used.

To flash the board with OpenOCD, use the `PROGRAMMER` variable:
```
    PROGRAMMER=openocd make BOARD=<nrf52 board> -C examples/hello-world flash
```

It is also possible to use the SWD interface of a ST-LINK/V2 in-circuit
debugger/programmer with OpenOCD to flash a nrf52 board:
```
    PROGRAMMER=openocd OPENOCD_DEBUG_ADAPTER=stlink make BOARD=<nrf52 board> -C examples/hello-world flash
```

*/
