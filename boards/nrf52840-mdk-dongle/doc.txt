/**
@defgroup    boards_nrf52840-mdk-dongle nRF52840 MDK USB Dongle
@ingroup     boards
@brief       Support for the nRF52840 MDK USB Dongle

### General information

Refer to [The makerdiary Github repo](https://github.com/makerdiary/nrf52840-mdk-usb-dongle)
or [The nrf52840-mdk-usb-dongle wiki](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/)
for more details about the device.

This is pretty much just the nRF52840 with a useful number of pins exposed for
your pleasure along with one RGB LED and an onboard antenna in a USB stick form
factor with room for headers on the sides.

Unlike similar sticks (like the @ref boards_nrf52840-mdk), it features no
dedicated programmer hardware but relies on direct USB communication with a
built-in bootloader.

The board features an RGB LED, a user/reset button as well as 12 configurable external pins.

### Pinout

\image html https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/assets/images/nrf52840-mdk-usb-dongle-pinout.png width=66%

### Links

- [nRF52840 documentation](https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html)
- [Schematic](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/hardware/nrf52840-mdk-usb-dongle-sch_v1_0.pdf)

### Flash the board

The board is flashed using its on-board UF2 boot loader.
The boot loader will present a mass storage device that has to be mounted to /media/MDK-DONGLE so
`uf2conv.py` can find it. If you have an auto-mounter installed this will happen automatically.

The rest of the process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by pressing the board's reset button while plugging the device into the USB port.

Readiness of the bootloader is indicated by the LED glowing green.

### Accessing STDIO

The usual way to obtain a console on this board is using an emulated USB serial port (CDC-ACM).
This is available automatically using the `stdio_cdc_acm` module,
unless any other stdio module is enabled.

On Linux systems with ModemManager < 1.10 installed,
`make term` will, in some setups, fail for a few seconds after the device has come up.
This is [fixed in its 1.12.4 version](https://gitlab.freedesktop.org/mobile-broadband/ModemManager/issues/164),
but should not be more than a short annoyance in earlier versions.

To ease debugging,
pins 0.6 and 0.7 are configured as RX and TX, respectively.
They provide stdio if no CDC-ACM is disabled,
and can be used as a custom UART otherwise.

 */
