Zolertia RE-Mote platform
============================================

The RE-Mote platform (Revision B) is a IoT Hardware development platform based
on TI's CC2538 system on chip (SoC), featuring an ARM Cortex-M3 with 512KB
flash, 32Kb RAM, double RF interface (Sub-1GHz CC1200 RF transceiver), and the
following goodies:

* ISM 2.4-GHz IEEE 802.15.4 & Zigbee compliant.
* ISM 868-, 915-, 920-, 950-MHz ISM/SRD Band.
* AES-128/256, SHA2 Hardware Encryption Engine.
* ECC-128/256, RSA Hardware Acceleration Engine for Secure Key Exchange.
* Power consumption down to 150nA using our shutdown mode.
* Flashing over BSL without requiring to press any button to enter bootloader mode.
* Built-in battery charger (500mA), Energy Harvesting and Solar Panels to be connected to standards LiPo batteries.
* Power input with wide range 3.7-16VDC.


Changes from Revision A
=========================

The list of changes are summarized in the [Wiki](https://github.com/Zolertia/Resources/wiki/RE-Mote:-Enhancements-from-Rev.A-to-Rev.B)

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
    * RF switch to programmatically drive either 2.4GHz or sub-1GHz to a single RP-SMA
    * RF 2.4GHz built-in in CC2538

And under work or pending at cc2538 base cpu:

    * Built-in core temperature and battery sensor.
    * CC1200 sub-1GHz radio interface.
    * Micro-SD external storage.
    * ADC
    * USB (in CDC-ACM).
    * uDMA Controller.

Requirements
============

 * Toolchain to compile RIOT for the CC2538
 * Drivers to enable your host to communicate with the platform
 * Built-in BSL programming over USB using cc2538-bsl (included)


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
The RE-Mote features a CP2104 serial-to-USB module, the driver is commonly found in most OS, but if required it can be downloaded
from <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>


### For the CC2538EM (USB CDC-ACM)
The RE-Mote has built-in support for USB 2.0 USB, Vendor and Product IDs are the following:

  * VID 0x0451
  * PID 0x16C8

On Linux and OS X this is straightforward, on windows you need to install the following driver:

<https://github.com/alignan/lufa/blob/remote-zongle/LUFA/CodeTemplates/WindowsINF/LUFA%20CDC-ACM.inf>

And replace the IDs accordingly.

### Device Enumerations
For the UART, serial line settings are 115200 8N1, no flow control.

Once all drivers have been installed correctly:

On windows, devices will appear as a virtual `COM` port.

On Linux, devices will appear under `/dev/`.

On OS X, `/dev/tty.SLAB_USBtoUARTx`.

On Linux:

* RE-Mote over CP2104: `ttyUSB0`
* RE-Mote over USB driver (in CDC-ACM): `ttyACMn` (n=0, 1, ....)

More Reading
============
1. [Zolertia RE-Mote website][remote-site]
2. [Zolertia Wiki page][zolertia-wiki]
3. [CC2538 System-on-Chip Solution for 2.4-GHz IEEE 802.15.4 and ZigBee applications (SWRU319B)][cc2538]
4. [CC1200 sub-1GHz RF transceiver][cc1200]

[remote-site]: https://zolertia.io/product/re-mote/ "Zolertia RE-Mote"
[zolertia-wiki]: https://github.com/Zolertia/Resources/wiki
[cc1200]: http://www.ti.com/product/cc1200     "CC1200"
[cc2538]: http://www.ti.com/product/cc2538     "CC2538"
