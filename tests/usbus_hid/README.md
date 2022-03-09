Expected result
===============

Connect you computer to the USB interface directly for the SoC and the
USB interface for power and debug.

Flash the device.

After flashing the device executing the command:

```
dmesg
```

should contain logs stating that a new USB HID device was found.

The output should look like the following:

```
[18579.559436] usb 1-9: new full-speed USB device number 7 using xhci_hcd
[18579.701474] usb 1-9: New USB device found, idVendor=1915, idProduct=521f, bcdDevice= 0.00
[18579.701481] usb 1-9: New USB device strings: Mfr=3, Product=2, SerialNumber=0
[18579.701484] usb 1-9: Product: Usb Hid Test Device
[18579.701487] usb 1-9: Manufacturer: RIOT-os.org
[18579.704613] hid-generic 0003:1915:521F.0008: hiddev0,hidraw6: USB HID v1.10 Device [RIOT-os.org Usb Hid Test Device] on usb-0000:00:14.0-9/input0
```

Note, that the endpoint (in this case hidraw6) might differ.

After successful initialization of USB HID one should be able to communicate
via USB HID simply by echoing input to the device.

Based on the screenshot above, an example command:

```
echo "Test" > /dev/hidraw6
```

The input string "Test" should be read by the test application and
printed to stdout.