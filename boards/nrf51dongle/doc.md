@defgroup    boards_nrf51dongle nRF51 Dongle
@ingroup     boards
@brief       Support for the Nordic nRF51 Dongle

## Overview:
The nRF51822 is a multi-protocol SoC ideally suited for Bluetooth® low energy
and 2.4GHz ultra low-power wireless applications from Nordic Semiconductor. The
nRF51822 is built around a 32-bit ARM® Cortex™ M0 CPU with 256KiB flash + 16KiB
RAM. The embedded 2.4GHz transceiver supports Bluetooth low energy as well as
2.4GHz operation.

Nordic's development kit contains two different boards:
- **pca10000**: a USB-dongle containing the nRF51822 and a RGB-Led
- **pca10005**: two a basic nRF51822 boards making the MCU pins available

While the pca10000 contains an on-board J-Link debugger, the pca10005 boards
have to be flashed/debugged using the (included) external J-Link device.

## Hardware:
![Nordic Semiconductor nrF51822 Development Kit](https://www.nordicsemi.com/-/media/Images/Products/DevKits/nRF51-Series/nRF51-Dongle-promo.png)

| MCU               | NRF51822QFAA                                                                  |
|:----------------- |:----------------------------------------------------------------------------- |
| Family            | ARM Cortex-M0                                                                 |
| Vendor            | Nordic Semiconductor                                                          |
| RAM               | 16KiB                                                                         |
| Flash             | 256KiB                                                                        |
| Frequency         | 16MHz                                                                         |
| FPU               | no                                                                            |
| Timers            | 3 (2x 16-bit, 1x 32-bit [TIMER0])                                             |
| ADCs              | 1x 10-bit (8 channels)                                                        |
| UARTs             | 1                                                                             |
| SPIs              | 2                                                                             |
| I2Cs              | 2                                                                             |
| Radio             | 2.4GHz BLE compatiple, +4dBm to -20 dBm output, -93 dBm RX sensitivity        |
| Vcc               | 1.8V - 3.6V                                                                   |
| Datasheet         | [Datasheet](https://infocenter.nordicsemi.com/pdf/nRF51822_PS_v3.3.pdf)       |
| Reference Manual  | [Reference Manual](https://infocenter.nordicsemi.com/pdf/nRF51_RM_v3.0.1.pdf) |


## Flashing the Device:
The nRF51822 PC10000 board is shipped with an on-board JLink debugger.
However the PC10004/5 board uses an external flash tool as seen in the Image
above. Under Linux, the easiest way flashing the nRF51822 is using the JLink
tool(version >4.85) from [Segger](http://www.segger.com/jlink-software.html).

To interact with the board just start the tool using the following command:
`# JLinkExe -device nrf51822`

For RIOT itself there is a Flash-Script available. When using any of the
Examples type in the following command:
`# Make -B clean flash`
