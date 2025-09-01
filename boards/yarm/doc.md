@defgroup   boards_yarm Acmesystems YARM board
@ingroup    boards
@brief      Support for the Acmesystems YARM board.

## Overview

Yarm is a smart and cost effective solution for system integrators to build
their own RF applications at 868 MHz avoiding all the hardware design costs
requested to start a new custom RF project.

Yarm integrates:

- a Microchip SAML21 low power MCU
- a Microchip ATA8510 radio module

See Acmesystems [product page](https://www.acmesystems.it/yarm) for more
information.

In the provided configuration, stdio is available via USB. The board could be
configured to provide it on the exposed UART instead, by defining

```
USEMODULE=stdio_uart
```

Depending on the connection to your PC, you will probably also need to set
PORT_LINUX to a different value (default is /dev/ttyACM0), for instance

```
PORT_LINUX=/dev/ttyUSB0
```

## Hardware

![yarm image](https://www.acmesystems.it/www/yarm/yarm-dev-yarm.jpg)

### MCU
| MCU           | ATSAML21J18B                                                                                           |
|:------------- |:------------------------------------------------------------------------------------------------------ |
| Family        | ARM Cortex-M0+                                                                                         |
| Vendor        | Microchip                                                                                              |
| RAM           | 32Kb                                                                                                   |
| Flash         | 256Kb                                                                                                  |
| Frequency     | up to 48MHz                                                                                            |
| FPU           | no                                                                                                     |
| Timers        | 8 (16-bit)                                                                                             |
| ADCs          | 1x 12-bit (20 channels)                                                                                |
| UARTs         | x 6 (shared with SPI and I2C)                                                                          |
| SPIs          | max 6 (see UART)                                                                                       |
| I2Cs          | max 6 (see UART)                                                                                       |
| Vcc           | 1.8V - 3.6V                                                                                            |
| Datasheet     | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/SAM_L21_Family_DataSheet_DS60001477C.pdf) |

### RADIO
| Transceiver   | ATA8510                                                                                                             |
|:------------- |:------------------------------------------------------------------------------------------------------------------- |
| Vendor        | Microchip                                                                                                           |
| Sensitivity   | -123dBm                                                                                                             |
| Output power  | -12dBm to +14.5dBm programmable in 0.4-dB steps ]                                                                   |
| Datasheet     | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/UHF-ASK-FSK-Transceiver-Product-Brief-DS00003077A.pdf) |

*NOTE:* not all MCU interfaces are available on YARM; see the
[pinout](https://www.acmesystems.it/pinout_yarm-dev) to know more.

## Implementation Status

| Device           | ID        | Supported | Comments                  |
|:---------------- |:----------|:--------- |:-------------             |
| MCU              | saml21    | partly    | PLL clock not implemented |
| Low-level driver | GPIO      | yes       |                           |
|                  | PWM       | yes       |                           |
|                  | UART      | yes       |                           |
|                  | I2C       | yes       |                           |
|                  | SPI       | yes       |                           |
|                  | USB       | yes       |                           |
|                  | RTT       | yes       |                           |
|                  | RTC       | yes       |                           |
|                  | RNG       | yes       |                           |
|                  | Timer     | yes       |                           |
|                  | ADC       | yes       |                           |


## Flashing the device

Both the MCU and the radio module are flashed using
[Atmel ICE](https://www.microchip.com/DevelopmentTools/ProductDetails/atatmel-ice),
with the help of a small adapter. The software used is edbg, bundled with RIOT.

[wiring](https://www.acmesystems.it/asquini/yarm_avr_programming/ice-wiring.jpg)

On Linux you might have to add a **udev** rule for Atmel ICE, like
```
bash
(
cat <<END
# Atmel-ICE JTAG/SWD in-circuit debugger
ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2141", MODE="664", GROUP="plugdev"
END
) | sudo tee -a /etc/udev/rules.d/99-atmel-ice.rules
sudo service udev restart
```

## Supported Toolchains

Recommended toolchain for SAML21 is [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded).
On Debian Buster, the default [gcc-arm-none-eabi](https://packages.debian.org/buster/gcc-arm-none-eabi)
package works well.
