Flashing on Linux using pickit-3
================================

This document describes how to enable flashing a pic32-wifire from Linux with a
PICkit3 using [pic32prog](https://github.com/sergev/pic32prog).

It will require flashing a specific firmware on the PICkit3.
As this can only be done from a Windows computer, that not many Linux users
have, the following steps explain how to setup a Windows VirtualBox virtual
machine and flash the PICkit3 from it.

Informations come from this comment

https://github.com/RIOT-OS/RIOT/pull/6092#issuecomment-261987955


Steps
-----

### Setup your computer

The steps were run on Ubuntu 16.04

* Install VirtualBox
* Add yourself to the 'vboxusers' group
  ```
  sudo usermod -a -G vboxusers your_username
  ```
* Reboot

### Setup Windows VM

You can download a free 90 days valid windows VirtualBox image from:

    https://developer.microsoft.com/en-us/microsoft-edge/tools/vms/

This document was tested on a Windows 10 VirtualBox image

* Create a machine in VirtualBox in File-Import Appliance.
* Start the virtual machine and wait until you are on the windows desktop
* Install VirtualBox Guest-additions for USB support
  * Doc: https://www.virtualbox.org/manual/ch04.html#additions-windows
* Shutdown the virtual machine
* Plug the PICkit3 flasher USB to your computer
* In the virtual machine settings, go to USB:
  * Select "Enable USB Controller" with "USB 1.1"
  * Click the small '+' sign on the right and add the Microchip devices.
  * Doc: https://www.virtualbox.org/manual/ch03.html#idm1640
* Start the virtual machine

### Downloads

Download and extract:

* PICkit3-Programmer http://ww1.microchip.com/downloads/en/DeviceDoc/PICkit3%20Programmer%20Application%20v3.10.zip

The steps were adapted from the archive's README:

* Extract `PICkit3 Programmer Application Setup v3.10.zip`
* Run `setup.exe`
* Run PICkit3
* Tools/Download PICkit Operating System
  * Select `c://Program Files/Microchip/PICkit3/PK3OSV020005.hex`
* Device will be flashed with the new firmware
* The GUI status should now say that the board is found
* Close the program (it crashes when closing it but it is ok)
* Turn off your VM


pic32prog
---------

Download and compile `pic32prog` flasher

https://github.com/sergev/pic32prog

Run the following command with PICkit3 connected in usb but without the
chipKIT-Wi-Fire board connected

```
$ pic32prog
Programmer for Microchip PIC32 microcontrollers, Version 2.0.221
    Copyright: (C) 2011-2015 Serge Vakulenko
      Adapter: PICkit3 Version 2.0.5
No device attached.

No target found.
```
You should see the `Adapter: PICkit3 Version 2.0.5` line.

### Troubleshooting

If you do not get the `Adapter: PICkit3 Version 2.0.5` line,
try running it with `sudo pic32prog`. If it works with `sudo` it is a
permission issue.

Add yourself to the `plugdev` group, add the following `udev` rule to
`/etc/udev/rules.d/26-microchip.rules` and reboot.

```
# Adapted from http://en.microstickplus.com/mplabx-on-linux
ATTR{idVendor}=="04d8", MODE="664", GROUP="plugdev"
```

Flashing
--------

* Connect the chipKIT-Wi-Fire to USB
* Connect the PICkit3 to JP1 ICSP holes
  * https://docs.creatordev.io/wifire/guides/wifire-programming/
  * The triangle `▶` goes into the port number 1 (a hole with a square around it)
    Opposite side of the JP1 ICSP text.

Run pic32prog again

```
$ pic32prog
Programmer for Microchip PIC32 microcontrollers, Version 2.0.221
    Copyright: (C) 2011-2015 Serge Vakulenko
      Adapter: PICkit3 Version 2.0.5
    Processor: MZ2048EFG100 (id 1720E053)
 Flash memory: 2048 kbytes
  Boot memory: 80 kbytes
Configuration:
    DEVCFG0 = fff6fff7
                     3 Debugger disabled
                     4 JTAG enabled
                     8 Use PGC2/PGD2
                   3   Flash ECC disabled, unlocked
    DEVCFG1 = 03743cb9
                     1 System PLL
                    8  Internal-external switch over enabled
                   0   Primary oscillator: External
                   4   CLKO output disabled
    DEVCFG2 = fff9b11a
                     2 PLL divider: 1/3
                    1  PLL input frequency range: 5-10 MHz
                  31   PLL feedback divider: x50
                 1     PLL postscaler: 1/2
              4        USB PLL input clock: 24 MHz
              8        Enable USB PLL
    DEVCFG3 = 86ffffff
               2       Default Ethernet pins
                       USBID pin: controlled by port
```

Now try flashing the chipKIT-Wi-Fire with RIOT default example compiled for
pic32-wifire with `pic32prog path/to/firmware.hex`


```
$ pic32prog bin/pic32-wifire/default.hex
Programmer for Microchip PIC32 microcontrollers, Version 2.0.221
    Copyright: (C) 2011-2015 Serge Vakulenko
      Adapter: PICkit3 Version 2.0.5
    Processor: MZ2048EFG100
 Flash memory: 2048 kbytes
  Boot memory: 80 kbytes
         Data: 117020 bytes
        Erase: done
Program flash: ########################### done
 Program boot: #### done
 Verify flash: ########################## done
  Verify boot: ### done
 Program rate: 6159 bytes per second
```
