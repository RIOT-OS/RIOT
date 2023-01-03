# Overview

This test represents a simple shell application that can be used to test
the USB board reset function (module `usb_board_reset`) via the USB CDC ACM
interface.

It can be used to reset the board to restart either the application or the
bootloader if supported using the `usb_board_reset` module.

The test application requires that the board provides USB peripherals
(feature `periph_usbdev`).

# Usage

Once the test application is flashed, the board should be detected. The output
of command
```
dmesg
```
should look like the following and should show the USB DC ACM interface:
```
[1745182.057403] usb 1-4.1.2: new full-speed USB device number 69 using xhci_hcd
[1745182.160386] usb 1-4.1.2: New USB device found, idVendor=1209, idProduct=7d01, bcdDevice= 1.00
[1745182.160390] usb 1-4.1.2: New USB device strings: Mfr=3, Product=2, SerialNumber=4
[1745182.160392] usb 1-4.1.2: Product: arduino-mkr1000
[1745182.160393] usb 1-4.1.2: Manufacturer: RIOT-os.org
[1745182.160395] usb 1-4.1.2: SerialNumber: 6B6C2CA5229020D8
[1745182.170982] cdc_acm 1-4.1.2:1.0: ttyACM0: USB ACM device
```
*Note*: The interface `ttyACM0` could be different depending on which other
        USB CDC ACM devices are already in use.

For boards that use the USB CDC ACM interface as STDIO (module `stdio_cdc_acm`),
this interface is used for the test application. Otherwise the test application
creates a simple USB CDC ACM interface without any functionality except the
USB board reset function.

When the USB CDC ACM interface is initialized, the `stty` command can be used
to reset the board, for example:
```
stty -F /dev/ttyACM0 raw ispeed 600 ospeed 600 cs8 -cstopb ignpar eol 255 eof 255
```
should reset the board and restart the application, while
```
stty -F /dev/ttyACM0 raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
```
should reset the board and start the bootloader. The latter requires that the
bootloader supports this.
