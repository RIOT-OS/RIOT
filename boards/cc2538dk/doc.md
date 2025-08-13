@defgroup    boards_cc2538dk CC2538DK
@ingroup     boards
@brief       Support for the Texas Instruments CC2538DK board.

## Overview

The [CC2538DK](http://www.ti.com/tool/cc2538dk) is Texas Instruments'
developer kit for the CC2538 SoC MCU, which combines an ARM Cortex-M3
microcontroller with an IEEE802.15.4 radio.

## Hardware

![cc2538dk](http://www.ti.com/diagrams/cc2538dk_cc2538dk_web_1.jpg)

| MCU               | CC2538SF53                |
|:----------------- |:------------------------- |
| Family            | ARM Cortex-M3             |
| Vendor            | Texas Instruments         |
| RAM               | 32KiB                     |
| Flash             | 512KiB                    |
| Frequency         | 32MHz                     |
| FPU               | no                        |
| Timers            | 4                         |
| ADCs              | 1x 12-bit (8 channels)    |
| UARTs             | 2                         |
| SPIs              | 2                         |
| I2Cs              | 1                         |
| Vcc               | 2V - 3.6V                 |
| Datasheet         | [Datasheet](http://www.ti.com/lit/gpn/cc2538) (pdf file) |
| Reference Manual  | [Reference Manual](http://www.ti.com/lit/pdf/swru319) |


##  Flashing and Debugging

By default, RIOT will attempt to flash the MCU via the USB UART using a
Python script named [cc2538-bsl](https://github.com/JelmerT/cc2538-bsl).
Hold down the SELECT pushbutton while pressing RESET to activate the MCU's
internal bootloader, then run:

`make flash`

Activating this bootloader is NOT enabled if the flash content is in factory
default state (e.g. after unboxing). To set the bits in the CCA accordingly you
have to follow the guidelines found [here](https://web.archive.org/web/20170610111337/http://processors.wiki.ti.com/index.php/CC2538_Bootloader_Backdoor).
To manage this first time access you have to download the
["Uniflash"](http://processors.wiki.ti.com/index.php/Category:CCS_UniFlash) tool
at TI's website.

Some Linux machines may not recognize the CC2538DK's vendor and product ID
automatically.
If a /dev/ttyUSBx device does not appear, try specifying these codes to the
FTDI driver manually:

`echo 0403 a6d1 > /sys/bus/usb-serial/drivers/ftdi_sio/new_id`

If the path `/sys/bus/usb-serial/drivers/ftdi_sio/` doesn't exist, you also
have to load the module `ftdi_sio` by hand.  Alternatively, you can install a
`udev` rule that configures this on device connection, see [this post on TI's
E2E site](https://e2e.ti.com/support/microcontrollers/c2000/f/171/p/359074/1843485#1843485)
for details.

RIOT will use /dev/ttyUSB1 by default, but if the UART is given a different
device name, you can specity it to RIOT using the PORT variable:

`make PORT=/dev/ttyUSB2 flash`

To flash using a Segger JLink JTAG adapter you need to install Segger's
JLinkExe tool, then specify `PROGRAMMER=jlink` when flashing:

`make PROGRAMMER=jlink flash`


# Mac OSX **El Capitan** users
Be prevented that you'll need to disable Apple's System Integrity Protection
to allow FTDI unsigned drivers to be loaded on your Mac.

@warning    Caution, turning off the SIP may compromise your systems security and integrity.
            See [developer.apple.com](https://developer.apple.com/documentation/security/disabling_and_enabling_system_integrity_protection) for details.


To do this, reboot in recovery mode, by pressing simultaneously `cmd + R`
while booting.
Then, on the recovery mode go to Utilities/Terminal and type:

```
# csrutil status
```

If you see something like:

```
System Integrity Protection status: enabled.
```

You should disable it by typing:

```
# csrutil disable
```

then reboot

```
# reboot
```

and be sure that your System Integrity Protection is disabled

```
$ csrutil status
System Integrity Protection status: disabled.
```

Afterwards you'll be able to install this
[driver](https://cdn.sparkfun.com/assets/learn_tutorials/7/4/FTDIUSBSerialDriver_v2_3.dmg).

If everything goes OK reboot your Mac and then edit
`/System/Library/Extensions/FTDIUSBSerialDriver.kext/Contents/Info.plist` with a
text editor.
Add the following block somewhere under `IOKitPersonalities`:
```
<key>TI_XDS100v3</key>
<dict>
<key>CFBundleIdentifier</key>
  <string>com.FTDI.driver.FTDIUSBSerialDriver</string>
  <key>IOClass</key>
  <string>FTDIUSBSerialDriver</string>
  <key>IOProviderClass</key>
  <string>IOUSBInterface</string>
  <key>bConfigurationValue</key>
  <integer>1</integer>
  <key>bInterfaceNumber</key>
  <integer>1</integer>
  <key>idProduct</key>
  <integer>42705</integer>
  <key>idVendor</key>
  <integer>1027</integer>
</dict>
```

Reboot again and then type:

```
$ sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext
```

If everything worked, the XDS will be enumerated as
`/dev/tty.usbserial-<serial-number>`
