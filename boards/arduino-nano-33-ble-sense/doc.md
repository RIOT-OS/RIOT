@defgroup    boards_arduino-nano-33-ble-sense Arduino Nano 33 BLE sense
@ingroup     boards
@brief       Support for the Arduino Nano 33 BLE Sense

### General information

The [Arduino Nano 33 BLE sense](https://store.arduino.cc/arduino-nano-33-ble-sense) board
is an opensource, micro development kit using the nRF52840 SoC.
This board provides 802.15.4 and BLE connectivity.

### Pinout

<img src="https://content.arduino.cc/assets/Pinout-NANOsense_latest.png"
     alt="pinout" style="height:800px;"/>

### Flash the board

Use `BOARD=arduino-nano-33-ble-sense` with the `make` command.<br/>
Example with `hello-world` application:
```
    make BOARD=arduino-nano-33-ble-sense -C examples/basic/hello-world flash
```

### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=arduino-nano-33-ble-sense -C examples/basic/hello-world term
```

### Internal devices

#### Supported

- hts221 (Temperature and Humidity sensor)
- apds9960 (Proximity, ambient light and gesture sensor)
- lps22hb (Pressure and Temperature sensor)

#### Not yet supported

- LSM9DS1 (9-axis inertial module - Accelerometer)
- MP34DT06JTR (Microphone)

### Hardware reference

- https://docs.arduino.cc/resources/pinouts/ABX00031-full-pinout.pdf
