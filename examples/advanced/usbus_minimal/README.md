# usbus_minimal example

This is a minimalistic example for RIOT's USB stack. The application will
initialize and start the USB stack. The stack is started without any USB
handlers, it should show up as an empty USB device on the host.

RIOT doesn't own any USB vendor and product ID. To compile this example, add
your own vendor and product ID to the makefile:

```
CFLAGS += -DCONFIG_USB_VID=0xYOURVID -DCONFIG_USB_PID=0xYOURPID
```

The example demonstrates basic USB communication between a host and a RIOT
based USB peripheral. Tools such as `lsusb` should display the device and
detailed information about the device such as descriptor strings.
