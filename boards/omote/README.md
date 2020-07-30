Oppila Microsystems omote platform -  http://www.oppila.in
==========================================================

The omote platform  is a IoT Hardware development platform based
on TI's CC2538 system on chip (SoC), featuring an ARM Cortex-M3 with 512KB
flash, 32Kb RAM, 2.4GHz RF interface , and the
following goodies:

* ISM 2.4-GHz IEEE 802.15.4 & Zigbee compliant.
* AES-128/256, SHA2 Hardware Encryption Engine.
* ECC-128/256, RSA Hardware Acceleration Engine for Secure Key Exchange.
* Power consumption down to 150nA using our shutdown mode.
* Built-in battery charger (500mA), Energy Harvesting and Solar Panels to be connected to standards LiPo batteries.
* Power input with wide range 3.7-16VDC.

Port Features
=============
In terms of hardware support, the following drivers have been implemented:

  * CC2538 System-on-Chip:
    * UART
    * Random number generator
    * Low Power Modes
    * General-Purpose Timers
    * I2C/SPI library
    * LEDs
    * Buttons
    * RF 2.4GHz built-in in CC2538

And under work or pending at cc2538 base cpu:

    * Built-in core temperature and battery sensor.
    * Micro-SD external storage.
    * ADC
    * USB (in CDC-ACM).
    * uDMA Controller.

Requirements
============

 * Toolchain to compile RIOT for the CC2538
 * Drivers to enable your host to communicate with the platform

Install a Toolchain
-------------------
The toolchain used to build is arm-gcc, to check if it is currently installed run:

    $ arm-none-eabi-gcc -v
    Using built-in specs.
    Target: arm-none-eabi
    Configured with: /scratch/julian/lite-respin/eabi/src/gcc-4.3/configure
    ...
    (skip)
    ...
    Thread model: single
    gcc version 4.3.2 (Sourcery G++ Lite 2008q3-66)

Else install from <https://launchpad.net/gcc-arm-embedded>


Drivers
-------
The omote features a CP2104 serial-to-USB module, the driver is commonly found in most OS, but if required it can be downloaded
from <https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx>


### For the CC2538EM (USB CDC-ACM)
The omote has built-in support for USB 2.0 USB, Vendor and Product IDs are the following:

  * VID 0x0451
  * PID 0x16C8

### Device Enumerations
For the UART, serial line settings are 115200 8N1, no flow control.

Once all drivers have been installed correctly:

On windows, devices will appear as a virtual `COM` port.

On Linux, devices will appear under `/dev/`.

On OS X, `/dev/tty.SLAB_USBtoUARTx`.

On Linux:

* omote over CP2104: `ttyUSB0`
* omote over USB driver (in CDC-ACM): `ttyACMn` (n=0, 1, ....)

More Reading
============
1. [Oppila Microsystems omote website](http://www.oppila.in)
2. [CC2538 System-on-Chip Solution for 2.4-GHz IEEE 802.15.4 and ZigBee applications (SWRU319B)][cc2538]

[cc2538]: http://www.ti.com/product/cc2538     "CC2538"
