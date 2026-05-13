@defgroup    boards_omote omote
@ingroup     boards
@brief       Support for the omote board

Oppila Microsystems omote platform -  <http://www.oppila.in>

The omote platform is a IoT Hardware development platform based
on TI's CC2538 system on chip (SoC), featuring an ARM Cortex-M3 with 512KB
flash, 32Kb RAM, 2.4GHz RF interface , and the
following goodies:

* ISM 2.4-GHz IEEE 802.15.4 & Zigbee compliant.
* AES-128/256, SHA2 Hardware Encryption Engine.
* ECC-128/256, RSA Hardware Acceleration Engine for Secure Key Exchange.
* On board sensors LDR, ADXL345 and BMP180
* Power input with wide range 3.7-30VDC.

## Port Features

In terms of hardware support, the following drivers have been implemented:

* CC2538 System-on-Chip:
  * UART
  * General-Purpose Timers
  * I2C/SPI library
  * LEDs
  * Buttons
  * RF 2.4GHz built-in in CC2538

## Required Drivers

The omote features a CP2104 serial-to-USB module, the driver is commonly found in most OS, but if required it can be downloaded
from <https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx>

### Device Enumerations

For the UART, serial line settings are 115200 8N1, no flow control.

Once all drivers have been installed correctly:

On Windows, devices will appear as a virtual `COM` port.

On Linux, devices will appear under `/dev/`.

On macOS, `/dev/tty.SLAB_USBtoUARTx`.

On Linux:

* omote over CP2104: `ttyUSB0`

## Flashing and Debugging

Currently RIOT supports flashing the omote using a Segger JLink JTAG
adapter or via USB, using the bootloader on the board.

### Flashing via JTAG

To be able to flash the board via JTAG you need to install Seggers JFlashLite
tool. Once you have this in place, you can simply flash by calling

```shell
JFlashLite
```

### Flashing via USB

The omote has built-in support to flash over USB using the BSL.It requires to unlock the bootloader by manually pressing the `BSL
button` and `reset button`

e.g

```shell
BOARD=omote make flash
```

The omote has 13 pinouts to connect boolean,digital and analog sensors based on I2C,UART,and SPI Protocols as well as other sensors or actuators you may need to connect.

### More Reading

1. [Oppila Microsystems omote website](http://www.oppila.in)
2. [CC2538 System-on-Chip Solution for 2.4-GHz IEEE 802.15.4 and ZigBee applications (SWRU319B)][cc2538]

[cc2538]: http://www.ti.com/product/cc2538     "CC2538"
