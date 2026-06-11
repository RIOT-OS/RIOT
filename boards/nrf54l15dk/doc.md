@defgroup    boards_nrf54l15dk nRF54L15-DK
@ingroup     boards
@brief       Support for the nRF54L15-DK board

## General information

The nRF54L15-DK is a development kit for the nRF54L15 SoC, an ultra-low-power
wireless MCU featuring an ARM Cortex-M33 application core running at 128 MHz,
a RISC-V coprocessor (FLPR), 1.5 MB of RRAM non-volatile memory, 256 KB of
SRAM and a 2.4 GHz multiprotocol radio (Bluetooth LE, 802.15.4, ...).

Currently, only the Cortex-M33 application core is supported by RIOT. The
radio can be used with IEEE 802.15.4 or BLE mode.

The board features four LEDs (active high), four user buttons/switches and a
reset button.

| Feature   | Pins                                                      |
|:--------- |:--------------------------------------------------------- |
| LEDs      | LED0: P2.09, LED1: P1.10, LED2: P2.07, LED3: P1.14        |
| Buttons   | BTN0: P1.13, BTN1: P1.09, BTN2: P1.08, BTN3: P0.04        |
| Console   | UARTE20, TX: P1.04, RX: P1.05 (115200 baud)               |

## Links

- [nRF54L15 DK web page](https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK)
- [nRF54L15 documentation](https://docs.nordicsemi.com/category/nrf54l-series)

## Flash the board

The board is flashed using the on-board SEGGER J-Link debugger.
Flashing the nRF54L15 requires a J-Link software version that supports the
device (V9.x).

The process is automated in the usual `make flash` target:

```shell
make BOARD=nrf54l15dk -C examples/basic/hello-world flash
```

## Accessing STDIO via UART

The STDIO is accessible via the USB port: the on-board debugger exposes two
virtual COM ports, the RIOT console is connected to the **second** one
(generally mapped to `/dev/ttyACM1` on a Linux host).

Use the `term` target to connect to the board serial port:

```shell
make BOARD=nrf54l15dk -C examples/basic/hello-world term
```
