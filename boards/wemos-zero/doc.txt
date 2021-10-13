/**
@defgroup    boards_wemos-zero WeMos Arduino Zero Clone
@ingroup     boards
@brief       Support for the WeMos Arduino Zero Clone board.

## Overview

The WeMos Zero is a cheap clone of Arduino Zero featuring a ATSAMD21G18A.
Unlike the original Zero, it does not include an embedded debugger/programmer.
Instead, the [UF2 Bootloader](https://github.com/microsoft/uf2-samdx1) is
used to program this board. The SAMD21 is a ARM Cortex-M0+ micro-controller.
It has 256KiB of flash memory and 32KiB of RAM.

Note: The official Arduino Zero using the native USB port and the Adafruit
Metro M0 Express should also work with board definition.

## Hardware

![wemos-zero](https://forum.arduino.cc/index.php?action=dlattach;topic=526753.0;attach=244160)


### MCU
| MCU           | ATSAMD21G18A                          |
|:------------- |:------------------------------------- |
| Family        | ARM Cortex-M0+                        |
| Vendor        | Atmel                                 |
| RAM           | 32KiB                                 |
| Flash         | 256KiB                                |
| Frequency     | up to 48MHz                           |
| FPU           | no                                    |
| Timers        | 6 (1x 16-bit, 2x 24-bit, 3x 32-bit)   |
| ADCs          | 6x 12-bit channels)                   |
| UARTs         | 2                                     |
| SPIs          | max 6 (see UART)                      |
| I2Cs          | max 6 (see UART)                      |
| Vcc           | 1.8V - 3.6V                           |
| Datasheet     | [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/SAM_D21_DA1_Family_Data%20Sheet_DS40001882E.pdf) |

### User Interface

1 LED:

| Device | PIN  |
|:------ |:---- |
| LED0   | PA17 |


## Flashing the device

Assuming the [UF2 Bootloader](https://github.com/microsoft/uf2-samdx1) is
installed, just run:

```
make BOARD=wemos-zero flash
```

Hint: You can you press the reset button twice to manually enter the bootloader.
This is helpful if the board has crashed (e.g. due to a failed assert() during
start up).

 */
