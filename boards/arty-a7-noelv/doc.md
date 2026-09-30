@defgroup    boards_arty-a7-noelv Gaisler NOEL-V Arty A7
@ingroup     boards
@brief       Support for the Gaisler NOEL-V RISC-V on the Digilent Arty A7
@author      Matvii Ivashchenko

## Overview

This board is the [Frontgrade Gaisler NOEL-V](https://www.gaisler.com/noel-v)
RISC-V processor running on the [Digilent Arty A7-100T][arty] (Xilinx
Artix-7 XC7A100T).

[arty]: https://digilent.com/reference/programmable-logic/arty-a7/start

No FPGA synthesis is required: Gaisler publishes ready-made bitstreams for
this board as [NOEL-ARTYA7][noel-artya7], and this board support targets the
`GP32L-SC` configuration from that package, which implements RV32IMAC with
the Zicbom cache maintenance instructions used by the GRETH driver. The
whole SoC lives in the FPGA fabric and runs at 40 MHz.

[noel-artya7]: https://www.gaisler.com/NOEL-ARTYA7

## Hardware

| Component     | Implementation                                     |
|:------------- |:-------------------------------------------------- |
| CPU           | NOEL-V RV32IMAC @ 40 MHz (GP32L-SC configuration)  |
| RAM           | 256 MiB SDRAM                                      |
| UART          | GRLIB APBUART                                      |
| Timer         | RISC-V ACLINT (mtime @ core clock / 2)             |
| GPIO          | GRLIB GRGPIO (LEDs LD4-LD7, buttons, switches)     |
| Ethernet      | GRLIB GRETH 10/100 MAC, on-board PHY               |

The four green LEDs LD4..LD7 are available as LED0..LED3; the RGB LEDs are
not connected in the example design. BTN0 is the main reset of the FPGA
design, so only BTN1..BTN3 are readable as GPIO inputs; the slide switches
SW0..SW3 are additionally exposed as BTN4..BTN7.

## Flashing

Download the FPGA configuration files from [NOEL-ARTYA7][noel-artya7] and
program the board with the Vivado script shipped in that archive:

```
vivado -mode tcl -notrace -source doprog.tcl
doprog GP32L-SC
```

Afterwards the application is loaded via JTAG using
[GRMON](https://www.gaisler.com/grmon):

```
grmon -digilent
grmon4> forward disable uart0
grmon4> load <application>.elf
grmon4> run
```

`forward disable uart0` keeps GRMON from capturing the UART output, so it
reaches the USB serial console (see below).

The RIOT build system uses GRMON as the default programmer for this board:

```
make BOARD=arty-a7-noelv flash
```

## Serial console

The APBUART console (115200 baud) is routed to the on-board FT2232HQ
USB-UART bridge, so the single USB cable on J10 carries both JTAG and the
console. The console shows up as the second USB serial interface (usually
`/dev/ttyUSB1`).

Switch SW3 selects between the RIOT application UART (SW3 = OFF) and the
GRMON debug link (SW3 = ON); keep it off to see the RIOT console.

## Ethernet

The GRETH MAC is wired to the on-board 10/100 PHY, no external hardware is
needed. The example design also enables the EDCL debug link on IP
192.168.0.51: the board answers ping on that address as soon as the
bitstream is loaded, which is a handy hardware check before any software
runs.

## Other configurations

The NOEL-ARTYA7 package also contains a `MC32L-SC` and a 64-bit `MC64-SC`
bitstream. Both use the same memory map and clock, but they are built from
NOEL-V configurations without the Zicbom extension, which the GRETH driver
relies on for DMA cache maintenance, so they are not supported here.
