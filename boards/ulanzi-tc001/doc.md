<!--
SPDX-FileCopyrightText: 2026 Benjamin Valentin
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards/ulanzi-tc001 Ulanzi TC001
@ingroup    boards_esp32
@brief      Support for Ulanzi TC001 digital Clock
@author     Benjamin Valentin <benjamin.valentin@ml-pa.com>

Information taken form https://github.com/rroels/ulanzi_tc001_hardware

## Flashing the Device {#esp32_wroom_32_flashing}

Flashing RIOT is quite easy. The board has a USB-C connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=ulanzi-tc001 ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32 as well as configuring and compiling
RIOT for ESP32 boards, see \ref esp32_riot.
