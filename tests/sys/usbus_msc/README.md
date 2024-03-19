# USBUS Mass Storage Class (MSC) test application

## Overview

This application uses RIOT USBUS device stack to associate a MTD device and
exports it as Mass Storage Class on your host computer.

**Please note:** RIOT doesn't own any USB vendor and product ID. The test
application therefore uses `USB_VID_TESTING=0x1209` as manufacturer ID and
`USB_PID_TESTING=0x7d01` as product ID. Do not use these IDs outside of
test environments! They MUST NOT be used on any device that is redistributed,
sold or manufactured, as they are not unique!

To compile this application with your own vendor and product ID, set the
variables `USB_VID` and `USB_PID` in the makefile or at the command line,
for example

```
USB_VID=1234 USB_PID=5678 BOARD=... make -C tests/pkg/tinyusb_cdc_msc
```

## Usage

Once the application is flashed, the device should start a shell.
By default, the USB device is not attached to your host computer.

From RIOT shell, you should be able to list your MTD devices available onboard
with the command:
`add_lun` without arguments

From there you can decide to export one or several MTD devices using the
command:
`add_lun`

Once all the wanted MTD devices are selected, user should start the USB device
by attaching it to the host with the command:
`usb_attach`

At this point, the USBUS stack will export the selected MTD devices.
Devices should appears under /dev/sdX entries.

If the board does not provide a MTD device, a MTD device can be emulated in RAM
for testing using module `mtd_emulated`:
```
USEMODULE=mtd_emulated BOARD=... make ...
```
The emulated MTD device has by default 64 sectors with 4 pages of 128 bytes
each, the minimum size to be able to create a partition with FAT file system.
These default parameters can be overridden e.g. by `SECTOR_COUNT`,
`PAGES_PER_SECTOR` and `PAGE_SIZE`:
```
CFLAGS='-DSECTOR_COUNT=128' USEMODULE=mtd_emulated BOARD=... make ...
```

**Notes:** Depending on the MTD device and the USB speed, this operation can take some times.

USB operation can be stopped at any time using:
`usb_detach`

## Limitations
There is a few limitations that should be taken into account:
- MTD with less that 512 bytes of memory cannot be used with MSC.
- MTD devices with flashpage size > 4096 need to be implemented and thus, won't work for now.

## TODO
- Add support for MTD devices with flashpage size > 4096
