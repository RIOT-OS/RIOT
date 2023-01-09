/**
@defgroup    boards_samd20-xpro Atmel SAM D20 Xplained Pro
@ingroup     boards
@brief       Support for the Atmel SAM D20 Xplained Pro board.

## Overview

The `SAMD20 Xplained Pro` is an ultra-low power evaluation board by Atmel
featuring an ATSAMD20J18 SoC. The SoC includes a SAMD20 ARM Cortex-M0+ micro-
controller. For programming the MCU comes with 32Kb of RAM and 256Kb of flash
memory.

## Hardware

![samd20-xpro image](https://keilpack.azureedge.net/content/Keil.SAMD20_DFP.1.1.1/Boards/Atmel/SAMD20-XPRO/Documents/SAMD20-XPRO_large.png)


### MCU
| MCU            | ATSAMD20J18A      |
|:-------------- |:--------------------------------- |
| Family         | ARM Cortex-M0+                    |
| Vendor         | Atmel                             |
| RAM            | 32 KiB                            |
| Flash          | 256 KiB                           |
| Frequency      | up to 48MHz                       |
| FPU            | no                                |
| Timers         | 8 (16-bit)                        |
| ADCs           | 1x 12-bit (20 channels)           |
| UARTs          | max 6 (shared with SPI and I2C)   |
| SPIs           | max 6 (see UART)                  |
| I2Cs           | max 6 (see UART)                  |
| Vcc            | 1.62V - 3.63V                     |
| Datasheet      | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/SAM_D20_%20Family_Datasheet_DS60001504C.pdf) |
| Board Manual   | [Board Manual](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42102-SAMD20-Xplained-Pro_User-Guide.pdf)|

### User Interface

1 User button and 1 LED:

| Device        | PIN   |
|:------------- |:----- |
| LED0          | PA14  |
| SW0 (button)  | PA15  |

## Flashing the device

Connect the device to your Micro-USB cable using the port labeled as *DEBUG
USB*.

The standard method for flashing RIOT to the samd20-xpro is using OpenOCD.

Refer to https://github.com/RIOT-OS/RIOT/wiki/OpenOCD for general
instructions on building OpenOCD and make sure "cmsis-dap" and "hidapi-libusb"
are enabled.

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
yaourt -S hidapi-git
yaourt -S openocd-git
# edit PKGBUILD, add "cmsis-dap hidapi-libusb" to "_features"
```

### Ubuntu
Although this refers to setting up the SAMR21, this guide is still very
helpful to understanding how to set up a solid RIOT development environment for
the SAMD20: http://watr.li/samr21-dev-setup-ubuntu.html

## Supported Toolchains

For using the samd20-xpro board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.

## Known Issues / Problems

 */
