/**
@defgroup    boards_nrf9160dk nRF9160DK
@ingroup     boards
@brief       Support for the nRF9160DK board

### General information

The nRF9160DK is a devboard based on nRF9160 MCU which offers LTE-M and NB-IoT
connectivity.

The board features four LEDs, four user buttons/switches and a reset button.

### Links

- [nRF9160dk web page](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk)
- [documentation and hardware description](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrf91_dk%2FUG%2Fnrf91_DK%2Fintro.html&cp=2_0_4)

### Flash the board

The board is flashed using JLink or nrfjprog software. Programs needs to
be installed.

The process is automated in the usual `make flash` target.

### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=nrf9160dk -C examples/hello-world term
```

 */