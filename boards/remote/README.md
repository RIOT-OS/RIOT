Zolertia Re-Mote platform
============================================

<a href="url"><img src="http://goo.gl/3AGZkI" align="center" height="320" ></a>

The Re-Mote platform is a IoT Hardware development platform based on TI's CC2538
system on chip (SoC), featuring an ARM Cortex-M3 with 512KB flash, 32Kb RAM,
double RF interface, and the following goodies:

* ISM 2.4-GHz IEEE 802.15.4 & Zigbee compliant.
* ISM 868-, 915-, 920-, 950-MHz ISM/SRD Band.
* AES-128/256, SHA2 Hardware Encryption Engine.
* ECC-128/256, RSA Hardware Acceleration Engine for Secure Key Exchange.
* Power consumption down to 3uA using our shutdown mode.
* Co-Processor to allow peripheral management, programming over BSL without requiring to press any button to enter bootloader mode.
* Built-in battery charger (500mA), Energy Harvesting and Solar Panels to be connected to standards LiPo batteries.
* Power input with wide range 2-26VDC.
* Built-in TMP102 temperature sensor
* Small form-factor (as the Z1 mote, half the size of an Arduino) 57x35 mm.


Port Features
=============
In terms of hardware support, the following drivers have been implemented:

  * CC2538 System-on-Chip:
    * UART
    * Random number generator
    * Low Power Modes
    * General-Purpose Timers.
    * ADC
    * LEDs
    * Buttons
    * Internal/external 2.4GHz antenna switch controllable by SW.

And under work or pending at cc2538 base cpu:

    * RF 2.4GHz built-in in CC2538 (PR #2198)
    * SPI/I2C library
    * Built-in core temperature and battery sensor.
    * TMP102 temperature sensor driver.
    * CC1120 sub-1GHz radio interface.
    * Micro-SD external storage.
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
The Re-Mote features a FTDI serial-to-USB module, the driver is commonly found in most OS, but if required it can be downloaded
from <http://www.ftdichip.com/Drivers/VCP.htm>


### For the CC2538EM (USB CDC-ACM)
The Re-Mote has built-in support for USB 2.0 USB, Vendor and Product IDs are the following:

  * VID 0x0451
  * PID 0x16C8

On Linux and OS X this is straightforward, on windows you need to install the following driver:

<https://github.com/alignan/lufa/blob/remote-zongle/LUFA/CodeTemplates/WindowsINF/LUFA%20CDC-ACM.inf>

And replace the IDs accordingly.

### Device Enumerations
For the UART, serial line settings are 115200 8N1, no flow control.

Once all drivers have been installed correctly:

On windows, devices will appear as a virtual `COM` port.

On Linux and OS X, devices will appear under `/dev/`.

On OS X:

* XDS backchannel: `tty.usbserial-<serial number>`
* EM in CDC-ACM: `tty.usbmodemf<X><ABC>` (X a letter, ABC a number e.g. `tty.usbmodemfd121`)

On Linux:

* Re-Mote over FTDI: `ttyUSB1`
* Re-Mote over USB driver (in CDC-ACM): `ttyACMn` (n=0, 1, ....)

More Reading
============
1. [Zolertia Re-Mote website][remote-site]
2. [CC2538 System-on-Chip Solution for 2.4-GHz IEEE 802.15.4 and ZigBee applications (SWRU319B)][cc2538]
3. [CC1120 sub-1GHz RF transceiver][cc1120]

[remote-site]: http://www.zolertia.io/products "Zolertia Re-Mote"
[cc1120]: http://www.ti.com/cc1120 "CC1120"
[cc2538]: http://www.ti.com/product/cc2538     "CC2538"
