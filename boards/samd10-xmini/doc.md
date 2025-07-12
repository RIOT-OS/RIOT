@defgroup    boards_samd10-xmini Atmel SAM D10 Xplained Mini
@ingroup     boards
@brief       Support for the Atmel SAM D10 Xplained Mini board.

## Overview

The `SAMD10 Xplained Mini` is an ultra-low cost evaluation board by Atmel
featuring an  SoC. The SoC includes a SAMD10 ARM Cortex-M0+ micro-
controller. For programming the MCU comes with 4Kb of RAM and 16Kb of flash
memory.

The samd10-xmini is available from various hardware vendors for ~8€ (as of
2020 October).

## Hardware

![samd10-xmini image](https://www.microchip.com/_ImagedCopy/ATSAMD10-XMINI.jpg)

### MCU
| MCU        | ATSAMD10D14AU         |
|:-----------|:--------------------- |
| Family     | ARM Cortex-M0+        |
| Vendor     | Atmel                 |
| RAM        | 4 kiB                 |
| Flash      | 16 kiB                |
| Frequency  | up to 48 MHz          |
| FPU        | no                    |
| Timers     | 3 (2 x 16-bit, 1 x 24-bit)       |
| ADCs       | 1x 10-bit (10 channels)          |
| DACs       | 1x 10-bit (1 channel)            |
| UARTs      | max 3 (shared with SPI and I2C)  |
| SPIs       | max 3 (see UART)                 |
| I2Cs       | max 3 (see UART)                 |
| Vcc        | 1.62V - 3.63V                    |
| Datasheet  | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42242-SAM-D10_Datasheet.pdf) |
| Board Manual | [Board Manual](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42387-ATSAMD10-Xplain-Mini_User-Guide.pdf) |

### User Interface

1 User button and 1 LED:

| Device | PIN  |
|:------ |:---- |
| LED0   | PA09 |
| SW0    | PA25 |

## Flashing the device

Connect the device to your Micro-USB cable using the port labeled as *DEBUG
USB*.

The standard method for flashing RIOT to the samd10-xmini is using [edbg](https://github.com/ataradov/edbg).
by calling: `make BOARD=samd10-xmini -C tests/leds flash`

Note that on Linux, you will need libudev-dev package to be installed.

Users can also use openOCD to flash and/or debug the board using:
`PROGRAMMER=openocd make BOARD=samd10-xmini -C tests/leds flash`

On Linux you will have to add a **udev** rule for hidraw, like
```
echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"' \
    | sudo tee -a /etc/udev/rules.d/99-usb.rules
sudo service udev restart
```

## Known Issues / Problems

### I2C

The I2C pins on the board do not have any pull-ups connected to them.
That means that running e.g. `i2c_scan` without any I2C slaves connected
will run into timeouts.
