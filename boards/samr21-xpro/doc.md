@defgroup    boards_samr21-xpro Atmel SAM R21 Xplained Pro
@ingroup     boards
@brief       Support for the Atmel SAM R21 Xplained Pro board.

## Overview

The `SAMR21 Xplained Pro` is a compact evaluation board by Atmel featuring a
ATSAMR21G18A SoC. The SoC includes a SAMD21 ARM Cortex-M0+ micro-controller
bundled with Atmel's AT86RF233, a 2.4GHz IEEE802.15.4 compatible radio. For
programming the MCU comes with 32Kb of RAM and 256Kb of flash memory.

The samr21-xpro is available from various hardware vendors for ~40EUR (as of
sep. 2014).

## Hardware

![samr21-xpro image](http://t3.gstatic.com/images?q=tbn:ANd9GcTdbgRUFLgLPWSYq6p26mR7wwikHnO4_vcEwRhwNZqmIHGUwVws3h2nfQ)


### MCU
| MCU        | ATSAMR21G18A      |
|:------------- |:--------------------- |
| Family | ARM Cortex-M0+    |
| Vendor | Atmel |
| RAM        | 32Kb |
| Flash      | 256Kb             |
| Frequency  | up to 48MHz |
| FPU        | no                |
| Timers | 6 (1x 16-bit, 2x 24-bit, 3x 32-bit)   |
| ADCs       | 1x 12-bit (8 channels)            |
| UARTs      | max 5 (shared with SPI and I2C)               |
| SPIs       | max 5 (see UART)                  |
| I2Cs       | max 5 (see UART)              |
| Vcc        | 1.8V - 3.6V           |
| Datasheet  | [Datasheet](http://www.atmel.com/Images/Atmel-42223-SAM-R21_Datasheet.pdf) |
| Board Manual   | [Board Manual](http://www.atmel.com/Images/Atmel-42243-SAMR21-Xplained-Pro_User-Guide.pdf)|

### User Interface

1 User button and 1 LED:

| Device | PIN |
|:------ |:--- |
| LED0   | PA19 |
| SW0 (button) | PA28 |





### Radio

The SAMR21 SoC includes an on-chip AT86RF233 radio. It is internally
connected via SPI and some GPIO pins and behaves in exact the same way as
externally connected SPI devices.

| Sensor | AT86RF233     |
|:------------- |:--------------------- |
| Type       | 2.4GHz IEEE802.15.4 radio |
| Vendor | Atmel |
| Datasheet  | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8351-MCU_Wireless-AT86RF233_Datasheet.pdf) |
| Connected to   | SPI_0         |
| Pin Config:    | |
| Device | SERCOM4           |
| MOSI       | PB30 (OUT, SPI mosi)  |
| MISO       | PC19 (IN, SPI miso)   |
| SCLK       | PC18 (OUT, SPI sclk)      |
| CS     | PB31 (OUT, GPIO output)       |
| IRQ        | PB00 (IN, GPIO external interrupt)        |
| RSTN       | PB15 (OUT, GPIO output)       |
| SLP_TR | PA20 (OUT, GPIO output)       |


## Implementation Status

Please refer to [this tracker](https://github.com/RIOT-OS/RIOT/issues/1646)
to see which projects are work in progress and who is the contact person for
this.

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | samd21    | yes| |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes       | |
|        | UART      | yes           | |
|        | I2C       | yes       | |
|        | SPI       | yes       | |
|        | ADC       | yes       | PA04 & PA05 disabled by default due to EDBG's use of them for UART, can be enabled from `periph_conf.h`|
|        | USB       | yes       | |
|        | RTT       | yes       | |
|        | RTC       | yes       |  |
|        | Timer     | yes           | |
| Radio Chip | AT86RF233 | yes       | using at86rf2xx driver |



## Flashing the device

Connect the device to your Micro-USB cable using the port labeled as *EDBG*.

The standard method for flashing RIOT to the samr21-xpro is using [edbg](https://github.com/ataradov/edbg).
by calling: `make BOARD=samr21-xpro -C tests/leds flash`

Note that on Linux, you will need libudev-dev package to be installed.

Users can also use openOCD to flash and/or debug the board using:
`PROGRAMMER=openocd make BOARD=samr21-xpro -C tests/leds flash`

On Linux you will have to add a **udev** rule for hidraw, like

```
bash
echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"' \
    | sudo tee -a /etc/udev/rules.d/99-usb.rules
sudo service udev restart
```

### Arch Linux
With yaourt:
```
yaourt -S libudev0
yaourt -S hidapi-git
yaourt -S openocd-git
# edit PKGBUILD, make sure "hidapi-libusb" is in "_features"
```

The **udev** rules for Arch differ a little from the example given above:
```
bash
echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="uucp"' \
    | sudo tee -a /etc/udev/rules.d/99-usb.rules
sudo udevadm control --reload
```

### Ubuntu
See http://watr.li/samr21-dev-setup-ubuntu.html


## Connecting multiple boards

If you have multiple boards connected to the same computer and want to flash,
this is what you can do:

First, run `make list-ttys`. This will show you a list of currently connected
boards, their serial numbers and the names of their TTYs:

    Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800002124', tty(s):
ttyACM0
    Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800002145', tty(s):
ttyACM1

knowing your serial number, you can flash, open a terminal or debug a
specific board like this:

    BOARD=samr21-xpro SERIAL="ATML2127031800002124" make flash
    BOARD=samr21-xpro SERIAL="ATML2127031800002124" make term
    BOARD=samr21-xpro SERIAL="ATML2127031800002124" make debug


## Accessing STDIO via UART

STDIO is available through the edbg debugger.

Use the `term` target to open a terminal:

    make BOARD=samr21-xpro -C examples/basic/hello-world term

RTS / CTS hardware flow control is available on `UART_DEV(0)` and
`UART_DEV(1)`. This is unavailable when using STDIO directly through
the debugger since it does not support it. Therefore to use hardware
flow control an external FTDI device must be connected to the board's
rx, tx, cts & rts matching pin headers, eg. for `UART_DEV(0)` to
`PA5`, `PA4`, `PA6` & `PA7` (respectively) on EXT1 headers.

## Known Issues / Problems

### 64 character input limit on EDBG UART
When using STDIO directly through the debugger UART, no more than 64 bytes can
be read at once. This does limit the length of shell command lines to 63 bytes
(since a newline is needed) when using a terminal that only transmits whole lines.
(e.g. pyterm).

### I2C
When connecting an I2C device and a logic analyzer to an I2C port at the same
time, the internal pull-up resistors are not sufficient for stable bus
operation. You probably have to connect external pull-ups to both bus lines. 10K
is a good value to start with.

### Stack sizes
The default stack sizes have not been tuned properly yet. If in doubt why
your application crashes try increasing the default stack sizes and use `ps` to
find out how much stack is being used.
Tracked in https://github.com/RIOT-OS/RIOT/issues/2228

### User Button
When using the SW0 user button as interrupt source it appears that this is
triggered when just tipping on the button and not really pressing it.

### Flashing might not work in Virtual Box with macOS as host
It might happen that flashing through OpenOCD works once inside Virtual Box.
But when you try to flash again, you could get a CMSIS-DAP related error. It
seems to only happen with USB 3.0 ports. You can take a look at
[Vagrant](http://en.wikipedia.org/wiki/Vagrant_%28software%29) and use a virtual
Linux to run the virtual RIOT, and flash from macOS.
