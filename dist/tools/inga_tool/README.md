inga_tool is a utility to reset the INGA and update the FTDI EEPROM
===================================================================

## Building
To build it, you need libudev and lipopt; and either libftdi 0.19 (or later) with libusb 0.1, or libftdi 1.0 (or later) with libusb 1.0.
Just typing

	$ make
	
should build the binary. I might add autotools support eventually.


Use it like this to reset an INGA node:

	./inga_tool -d /path/to/usb/serial/device -r

The following will flash the EEPROM (WARNING, not extensively tested):

	./inga_tool -d /path/to/usb/serial/device -f -v

You can also read out the EEPROM:
	
	./inga_tool -d /path/to/usb/serial/device -s

It is also possible to choose a device based on the USB serial ID:

	./inga_tool -u <serial> -r

### Reset
This is used to go into the bootloader without the need to reset the device manually.
You'll need an up-to-date bootloader for this to work.
In addition, flashing the EEPROM of INGA is required before using the reset functionality.

### Flashing
Right now no options are supported for flashing, that will be something for the future.
The following changes will be made:
 * manufacturer -> "IBR"
 * product -> "INGA"
 * cbus functions are set to
 
 ```
 CBUS0 TXLED
 CBUS1 RXLED
 CBUS2 TXDEN
 CBUS3 IOMODE
 CBUS4 SLEEP
 ```

Unchanged are VID/PID and the serial.

## OS X
inga_tool works partly also with OS X.
Selecting a device ist only possible with option `-u` (serial number).
It is not possible to unload the FTDI driver for a specific USB-device.
Also you need to be a superuser to unload the FTDI driver (***Warning: this disconnects all FTDI devices***).

But if you need so you can use 

	sudo kextunload -bundle com.apple.driver.AppleUSBFTDI

to unload the driver and

	sudo kextload -bundle com.apple.driver.AppleUSBFTDI

to reactivate the driver after using inga_tool.
These changes are only permanent until next reboot.
