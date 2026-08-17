@defgroup    boards_zedboard_noelv Gaisler NOEL-V ZedBoard
@ingroup     boards
@brief       Support for the Gaisler NOEL-V RISC-V on the ZedBoard FPGA
@author      Matvii Ivashchenko

## Overview

This board is the [Frontgrade Gaisler NOEL-V](https://www.gaisler.com/noel-v)
64-bit RISC-V (RV64IMAC) reference design synthesized for the
[Avnet ZedBoard][zedboard] (Xilinx Zynq-7020).

[zedboard]: https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/zedboard/
The processor and all peripherals are implemented in the
programmable logic (PL); the Zynq processing system (PS) only supplies the
clock and the DDR memory controller.

## Hardware

| Component     | Implementation                                    |
|:------------- |:------------------------------------------------- |
| CPU           | NOEL-V RV64IMAC @ 40 MHz (GRLIB reference design) |
| RAM           | 128 MiB DDR3 (shared with the Zynq PS)           |
| UART          | GRLIB APBUART                                     |
| Timer         | RISC-V ACLINT (mtime @ core clock / 2)            |
| GPIO          | GRLIB GRGPIO (LEDs LD0-LD7, buttons, switches)    |
| Ethernet      | GRLIB GRETH 10/100 MAC, RMII PHY on PMOD JD       |

## Flashing

The board is programmed via JTAG using [GRMON](https://www.gaisler.com/grmon):

```
grmon -digilent
grmon4> forward disable uart0
grmon4> load <application>.elf
grmon4> run
```

`forward disable uart0` keeps GRMON from capturing the UART output, so it
reaches the physical pins (see below).

The RIOT build system uses GRMON as the default programmer for this board:

```
make BOARD=zedboard-noelv flash
```

## Serial console

The APBUART console (115200 baud) is routed to Pmod connector JC. JC is a
differential Pmod, so the pair sits on adjacent pins:

| Pmod JC pin | Signal   | Connect to        |
|:----------- |:-------- |:----------------- |
| 1 (JC1_P)   | board RX | adapter TX        |
| 2 (JC1_N)   | board TX | adapter RX        |
| 5 or 11     | GND      | adapter GND       |

Switch SW3 must be in the low position to route the APBUART to the pins.
Switch SW3 selects between the RIOT application UART (SW3 = 0) and the
GRMON debug link (SW3 = 1).
