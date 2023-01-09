/**
@defgroup    boards_spark-core Spark-Core
@ingroup     boards
@brief       Support for the spark-core board

## Overview

Spark's hardware dev kit, the Spark Core, provides everything you need to
build a connected product. We combine a powerful ARM Cortex M3 micro-controller
with a Wi-Fi module to get you started quickly. The design is open source, so
when you're ready to integrate the Core into your product, you can.

## Hardware

![Spark-Core image]
(https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/spark-core.jpg)

Link to [product website](http://docs.spark.io/hardware/).

### MCU
| MCU        | STM32F103CB       |
|:------------- |:--------------------- |
| Family | ARM Cortex-M3     |
| Vendor | ST Microelectronics   |
| RAM        | 20Kb  |
| Flash      | 128Kb             |
| Frequency  | up to 72MHz (using the on-board 8MHz Oszillator of the ST- Link) |
| FPU        | no                |
| Timers | 10 (9x 16-bit, 1x 32-bit [TIM2])  |
| ADCs       | 4x 12-bit         |
| UARTs      | 5                 |
| SPIs       | 3                 |
| I2Cs       | 2                 |
| Vcc        | 2.0V - 3.6V           |
| Datasheet  | [Datasheet](https://github.com/spark/core/blob/master/Datasheets/ST_STM32F103CB.pdf) |
| Reference Manual | [Reference Manual](xxx) |
| Programming Manual | [Programming Manual](xxx) |
| Board Manual   | [Board Manual](http://docs.spark.io/hardware/)|

# Hardware connection

First of all make sure the your spark core device is detected and can be
accessed properly. In Linux you might have to adept your udev rules accordingly:

```
$ cat 50-openmoko.rules
#Bus 001 Device 022: ID 1d50:607f OpenMoko, Inc.

SUBSYSTEMS=="usb", ATTRS{idVendor}=="1d50", ATTRS{idProduct}=="607f", \
    MODE:="0666", \
    SYMLINK+="openmoko_%n"

# If you share your linux system with other users, or just don't like the
# idea of write permission for everybody, you can replace MODE:="0666" with
# OWNER:="yourusername" to create the device owned by you, or with
# GROUP:="somegroupname" and mange access using standard unix groups.

sudo cp 50-openmoko.rules  /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

now replug the usb cable and flash.

# Flashing

To flash RIOT to the spark core you need to install `dfu-util` (tested with
0.7 and 0.8).

Power the device and press the the `mode` button until the LED flashes
yellow/orange: now the device is ready to get flashed.

# Firmware upgrade

To run RIOT it is required that you run the newest spark bootloader on your
core:
from https://community.spark.io/t/replace-core-firmware-with-riot-os/7007/7

```
Here is the core-firmware branch for this:
https://github.com/spark/core-firmware/tree/bootloader-patch-update1
However care should be taken not to disturb the patch update process by
accidentally removing usb power or through reset.
Following is the procedure to update the bootloader-patch
1)cd core-firmware
2)git pull
3)git checkout bootloader-patch-update
4)cd build [No need to run make]
5)Enter usb dfu/bootloader mode => Yellow flashes
6) dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D core-firmware.bin
7)Upon execution, the bootloader should be updated within a second or two and
normal core-firmware should start executing.
8)Congrats! your core is loaded with latest bootloader without the hassle of
going through ST-Link JTAG programming
```

Build and flash
===============

1. clone RIOT-OS
2. cd to e.g. `examples/hello_world`
3. enter `BOARD=spark-core make clean all flash`

Use the UART
============

1. RIOT is using the UART PINs on the board, they are labeled as `RT` and
`TX`.
2. Connect your TTY to them and `BOARD=spark-core make term`.
3. In a different terminal enter `BOARD=spark-core make reset`

Pin mapping in RIOT
===================

The following image shows RIOT's static pin mapping chosen for this board.
This mapping is completely arbitrary, it can be adjusted in
`boards/spark-core/include/periph_conf.h`
![Pin Mappin](http://docs.spark.io/assets/images/spark-pinout.png)

 */
