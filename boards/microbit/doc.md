@defgroup    boards_microbit BBC micro:bit
@ingroup     boards
@brief       Support for the BBC micro:bit

## Overview

The [micro:bit](https://www.microbit.co.uk/) was designed by the BBC and
released in 2015. The boards was distributed to all 11-12 year old children
throughout the UK.

The board is based on the Nordic nRF51822 SoC, featuring 16KiB of RAM, 256KiB
of ROM, and a 2.4GHz radio, that supports Bluetooth Low Energy (BLE) as well as
a Nordic proprietary radio mode.

Additionally the boards features 2 buttons, a 5x5 LED matrix, a MAG3110
3-axis magnetometer, and a MMA8653 3-axis accelerometer.


## Hardware

![micro:bit](https://github.com/RIOT-OS/RIOT/wiki/images/board_microbit.png)

| MCU               | NRF51822QFAA                      |
|:----------------- |:--------------------------------- |
| Family            | ARM Cortex-M0                     |
| Vendor            | Nordic Semiconductor              |
| RAM               | 16KiB                             |
| Flash             | 256KiB                            |
| Frequency         | 16MHz                             |
| FPU               | no                                |
| Timers            | 3 (2x 16-bit, 1x 32-bit [TIMER0]) |
| ADCs              | 1x 10-bit (8 channels)            |
| UARTs             | 1                                 |
| SPIs              | 2                                 |
| I2Cs              | 2                                 |
| Vcc               | 1.8V - 3.6V                       |
| Reference Manual  | [Reference Manual](http://infocenter.nordicsemi.com/pdf/nRF51_RM_v3.0.pdf) |


##  Flashing and Debugging

There are two possibilities to flash the board: using the default ARM DAPLink
or you can flash the board using Segger's JLink.

### DAPLink

The [DAPLink interface](https://www.mbed.com/en/development/hardware/prototyping-production/daplink/daplink-on-kl26z/) is the default way to flash the board and
works out of the box. When you plug the board to your host computer, it shows up
as a flash drive. To flash the board, you can simply copy your compiled `.hex`
file onto the board, and that's it.

The `micro:bit` port comes with a little script that does this automatically,
so you can flash the board as usual with

```
BOARD=microbit make flash
```

The DAPLink interface provides however not means for debugging the board.

### JLink

Recently, Segger released a JLink firmware for the interface MCU on the
`micro:bit`. You have to follow [these instructions](https://www.segger.com/bbc-micro-bit.html) to flash the JLink firmware on your `micro:bit`. Don't worry,
the process is very simple and you can revert the firmware back to the DAPLink
default anytime ([as described here](https://www.mbed.com/en/development/hardware/prototyping-production/daplink/daplink-on-kl26z/)).

Once you have flashed the JLink firmware, you can flash the board like this:

```
BOARD=microbit PROGRAMMER=jlink make flash
```

With the JLink firmware, you can now also do in-circuit debugging etc.

**Note: The current version of the JLink firmware
(JLink_OB_BBC_microbit_16-07-29.hex) does not support any serial port over USB,
so you can not use the RIOT shell with this firmware.**


### QEMU emulation

The microbit can be partly emulated by QEMU.

This requires at least QEMU 4.0 with ARM platform support enabled.

*NOTE*: not all peripherals are emulated. See
[this](https://wiki.qemu.org/Features/MicroBit) page for an overview.
E.g., there's no emulation for the radio, thus applications using that will
fail.

Use it like this:

    $ cd examples/basic/hello-world
    $ BOARD=microbit make clean all -j4
    $ EMULATE=1 BOARD=microbit make term

## Display

The 5x5 LED matrix display can be driven using the @ref boards_common_microbit.
