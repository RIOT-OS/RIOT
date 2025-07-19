@defgroup    boards_arduino-nano-33-ble Arduino Nano 33 BLE
@ingroup     boards
@brief       Support for the Arduino Nano 33 BLE

### General information

The [Arduino Nano 33 BLE](https://store.arduino.cc/arduino-nano-33-ble) board
is an opensource, micro development kit using the nRF52840 SoC.
This board provides 802.15.4 and BLE connectivity.

### Pinout

<img src="https://content.arduino.cc/assets/Pinout-NANOble_latest.png"
     alt="pinout" style="height:800px;"/>

### Flash the board

Use `BOARD=arduino-nano-33-ble` with the `make` command.<br/>
Example with `hello-world` application:
```
    make BOARD=arduino-nano-33-ble -C examples/basic/hello-world flash
```

### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=arduino-nano-33-ble -C examples/basic/hello-world term
```
