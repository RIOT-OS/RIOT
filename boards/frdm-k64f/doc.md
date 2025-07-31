@defgroup    boards_frdm-k64f NXP FRDM-K64F Board
@ingroup     boards
@brief       Support for the NXP FRDM-K64F

## Overview
The board has a K64F Kinetis MCU and is supported by `cpu/kinetis_common`.

## Hardware

![frdm-k64f](https://www.nxp.com/assets/images/en/dev-board-image/BOARD-FRDM-K64F-PRODUCT-SHOT.png)

| MCU               | MK64FN1M0VLL12        |
|:----------------- |:--------------------- |
| Family            | ARM Cortex-M4F        |
| Vendor            | Freescale             |
| RAM               | 256KiB                |
| Flash             | 1024KiB               |
| Frequency         | 120MHz                |
| FPU               | yes                   |
| Timers            | yes                   |
| ADCs              | yes                   |
| UARTs             | yes                   |
| SPIs              | yes                   |
| I2Cs              | yes                   |
| Ethernet          | WIP                   |
| USB               | WIP                   |
| LPM               | TODO                  |
| DAC               | TODO                  |
| Vcc               | TODO                  |
| Reference Manual  | TODO                  |

The board has an integrated debugger adapter (k20dx128) with the firmware from
ARMmbed.
There are three interfaces available:
* Drag-n-drop programming over Mass Storage Device
* USB Serial Port (/dev/ttyACMx) connected to a K64F UART interface
* CMSIS-DAP for debugging with e.g. OpenOCD

**Please update the firmware to version 0221, see below.**

## OpenOCD
The latest (01.07.2015) OpenOCD version (also current git) does not support
the K64F. There is a [patch](http://openocd.zylin.com/#/c/2773/) which makes
flashing and debugging possible. A refactored and rebased version can be found
[here](https://github.com/jfischer-phytec-iot/openocd/tree/wip%40phytec).<br>

## Flashing the Device
#### Over OpenOCD
make BOARD=frdm-k64f flash

#### Over MSD
Simply mount the mbed Mass Storage Device und copy new RIOT firmware.
See also the ARMmbed
[instructions](https://developer.mbed.org/platforms/FRDM-K64F/#getting-started-
with-mbed).

## Updating the Bootloader
A update of CMSIS-DAP firmware is necessary to using the board with OpenOCD.
A good step by step guide is available
[here](https://developer.mbed.org/handbook/Firmware-FRDM-K64F).

## I can't flash the device with OpenOCD anymore...
When a flash process has failed or a wrong firmware has been programmed, the
following OpenOCD message may appear:
```
START...
Info : SWD IDCODE 0x2ba01477
Info : SWD IDCODE 0x2ba01477
Error: Failed to read memory at 0xe000ed00
Examination failed, GDB will be halted. Polling again in 300ms
    TargetName         Type       Endian TapName            State
--  ------------------ ---------- ------ ------------------ ------------
 0* k64f.cpu           cortex_m   little k64f.cpu           unknown
START...
Warn : *********** ATTENTION! ATTENTION! ATTENTION! ATTENTION! **********
Warn : ****                                                          ****
Warn : **** Your Kinetis MCU is in secured state, which means that,  ****
Warn : **** with exception for very basic communication, JTAG/SWD    ****
Warn : **** interface will NOT work. In order to restore its         ****
Warn : **** functionality please issue 'kinetis mdm mass_erase'      ****
Warn : **** command, power cycle the MCU and restart OpenOCD.        ****
Warn : ****                                                          ****
Warn : *********** ATTENTION! ATTENTION! ATTENTION! ATTENTION! **********
in procedure 'reset'
in procedure 'ocd_bouncer'
```
Solution 1:
+ Hold the reset button and execute make BOARD=frdm-k64f flash in a project
directory
+ Release reset button just after "Info : MDM: Chip is unsecured.
Continuing."

Solution 2:
+ Comment out line `reset_config srst_only srst_nogate connect_assert_srst`
in `boards/frdm-k64f/dist/openocd.cfg`
+ Execute make BOARD=frdm-k64f flash in a project directory

Solution 3:
+ Ask Deep Thought
