@defgroup    boards_mulle Eistec Mulle
@ingroup     boards
@brief       Support for Eistec Mulle IoT boards

![Mulle](https://web.archive.org/web/20161213064400im_/http://eistec.github.io/images/mulle-small.jpg)

The Mulle is a miniature wireless Embedded Internet System suitable for
wireless sensors connected to the Internet of Things, and designed for rapid
prototyping.

 - [Official homepage (Archived)]
   (https://web.archive.org/web/20161213064400/http://www.eistec.se/mulle)
 - [Eistec wiki](https://github.com/eistec/mulle/wiki)

Use `BOARD=mulle` for building RIOT for this platform.

## Components

https://github.com/eistec/mulle/wiki/Datasheets contains a list of relevant
documentation for the components.

| MCU                   | MK60DN512VLL10 – Cortex-M4                                                                                                        |
|:--------------------- |:--------------------------------------------------------------------------------------------------------------------------------- |
| RAM                   | 64KiB                                                                                                                             |
| Flash                 | 512KiB                                                                                                                            |
| radio chipset         | AT86RF212B, sub-GHz IEEE802.15.4 transceiver, similar to the AT86RF233                                                            |
| external flash memory | Micron M25P16 2 MiB external NOR flash, used for storing configuration, measurements and other slow changing non-volatile data    |
| external FRAM memory  | Cypress/Ramtron FM25L04B 512B external F-RAM, used for storing counters and other rapidly changing non-volatile data              |
| accelerometer         | ST micro LIS3DH MEMS accelerometer                                                                                                |


## Layout



## Implementation Status

The Mulle board is supported by mainline RIOT. See the below table for
software support status for the different components.

| Device            | ID                | Supported     | Comments                                              |
|:----------------- |:----------------- |:------------- |:----------------------------------------------------- |
| MCU               | MK60DN512VLL10    | partly        | See below                                             |
| Low-level driver  | GPIO              | yes           |                                                       |
|                   | PWM               | yes           |                                                       |
|                   | UART              | yes           |                                                       |
|                   | I2C               | yes           |                                                       |
|                   | SPI               | yes           | Slave mode unsupported                                |
|                   | USB               | no            | [PR#3890](https://github.com/RIOT-OS/RIOT/pull/3890)  |
|                   | RTT               | yes           |                                                       |
|                   | RNG               | yes           |                                                       |
|                   | timer             | yes           | uses LPTMR module for TIMER_0 (used by xtimer), 32.768 kHz tick rate. PIT for additional timers, F_BUS tick rate (48 MHz default) |
|                   | PM/LLWU           | in progress   | [PR#2605](https://github.com/RIOT-OS/RIOT/pull/2605)  |
| Radio Chip        | AT86RF212B        | yes           |                                                       |
| Accelerometer     | LIS3DH            | yes           |                                                       |
| Flash             | M25P16            | yes           |                                                       |
| FRAM              | FM25L04B          | yes           |                                                       |

## Toolchains

See [ARM Family](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM), and
[Eistec wiki - Installing-toolchain (GCC)](https://github.com/eistec/mulle/wiki/Installing-toolchain-%28GCC%29)

Working:
* [gcc-arm-embedded](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM#gcc-arm-embedded-toolchain)
* Clang 3.4 - Clang 4.0 - Install procedure not documented but pretty
straightforward, Makefiles and build system is fully functional with Clang.

### Programming and Debugging

See [Eistec wiki](https://github.com/eistec/mulle/wiki).
