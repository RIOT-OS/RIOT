# TinyUSB package test application

## Overview

This application uses the tinyUSB device stack to emulate a mass storage
device (MSC) with a communication interface (CDC).

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

Once the application is flashed, the device should be mounted when it is
connected to a host. That is,

- the mass storage interface is mounted as volume `TinyUSB MSC` in the
  operating system and
- the communication interface is mounted as a serial device, for example
  as `/dev/ttyACM0` on Linux.

It should then be possible

1. to read from and write to the mass storage with the usual file operations
   of the operating system
2. to connect to the serial interface of the device with a terminal program, e.g.
   ```
   python -m serial.tools.miniterm /dev/ttyACM0 115200
   ```
   and get the entered characters sent back.

The test application uses LED0, if present, to indicate the status of the
device by blinking at different frequencies.
