# usb_printf example

This example uses the USB CDC ACM interface as stdio, so `printf()`/`puts()`
output is sent over USB. It prints a message once per second without requiring a
timer periph to be available.

## Usage

Build and flash (put the board into BOOTSEL mode first if needed):

```shell
make BOARD=rpi-pico-2-arm flash
```

Then open the USB CDC ACM interface:

```shell
make BOARD=rpi-pico-2-arm term
```

The output should appear on `/dev/ttyACM0`.

## Notes

- `USB_VID`/`USB_PID` default to the RIOT testing IDs (`1209`/`7d01`). They must
  not be used on redistributed devices.
