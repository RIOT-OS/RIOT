@defgroup    boards_yunjia-nrf51822 Yunjia NRF51822
@ingroup     boards
@brief       Support for the Yunjia NRF51822 board

## Overview

The NRF51822 module by Yunjia Technology is a minimalistic but very low-
priced module utilizing Nordics NRF51822QFAA SoC. The SoC features 16Kb of RAM,
256Kb of flash ROM and comes on top of the usual micro-controller peripherals
with a 2.4GHz radio that supports both Nordics proprietary ShockBurst as well as
Bluetooth Low Energy (BLE).

The board is available for example on [ebay](https://www.ebay.com/sch/i.html?_nkw=nrf51822)
or at [aliexpress](http://www.aliexpress.com/wholesale?SearchText=nrf51822&catId=0&initiative_id=SB_20140804233951) for something around 8-10 USD.

## Hardware

![yunjia-nrf51822](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/yunjia-nrf51822-full.jpg)

| MCU        | NRF51822QFAA      |
|:------------- |:--------------------- |
| Family | ARM Cortex-M0     |
| Vendor | Nordic Semiconductor  |
| RAM        | 16Kb  |
| Flash      | 256Kb             |
| Frequency  | 16MHz |
| FPU        | no                |
| Timers | 3 (2x 16-bit, 1x 32-bit [TIMER0]) |
| ADCs       | 1x 10-bit (8 channels)        |
| UARTs      | 1                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| Vcc        | 1.8V - 3.6V           |
| Datasheet  | [Datasheet](https://infocenter.nordicsemi.com/pdf/nRF51822_PS_v3.3.pdf) |
| Reference Manual | [Reference Manual](https://infocenter.nordicsemi.com/pdf/nRF51_RM_v3.0.1.pdf) |


##  Flashing and Debugging
The Yunjia NRF51822 module comes without any on-board programming and
flashing capabilities. It supports however to be programmed using of-the-shelf
programmers as Segger's JLink or STM's STLink.

A very simple and affordable way to program and debug the Yunjia module is to
the integrated ST-Link/V2 programmer of any STM32Fx-discovery board. The needed
steps are described in the following sections. If you want to use a stand-alone
ST-Link adapter, you just simply have to alter the wiring to fit for your
programmer, the software part is identical.

### Hardware
First of all make sure the your ST-Link device is detected and can be
accessed properly. In Linux you might have to adept your `udev` rules
accordingly:
```
wget https://raw.githubusercontent.com/texane/stlink/master/etc/udev/rules.d/49-stlinkv2.rules
sudo cp 49-stlinkv2.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```
now replug the usb cable and flash.

Have a look at the 'Setting up udev rules' section in this
[README file](https://github.com/texane/stlink/)
if you need help.

Second you need to enable the stand-alone ST-Link mode of the discovery board
by removing the two `CN2` jumpers, found somewhere in the upper right part of
the board. This disconnects the ST-Link programmer from the micro-controller
part of the port and enables direct access through the pin-header `CN3`, also
labeled `SWD`.

The Yunjia NRF51822 module supports the Serial Wire Debug (SWD) interface. To
access the device the following four lines need to be connected with the
STM32x-discovery board:
```
                 Yunjia module    STM32Fx-discovery
common ground:       GND <-----------> GND
supply voltage:      VDD <-----------> 3V
SWD clock:           SWD <-----------> SWCLK (CN3, pin2)
SWD data I/O:      SWDIO <-----------> SWDIO (CN3, pin4)
```

The following image shows the wiring for an STM32F0-discovery board (although
not the most beautiful...):

![Connecting the Yunjia module to an STM32F0-discoveryboard](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/yunjia-wiring.jpg)


### Software
Debugging and programming the Yunjia module works well with [[OpenOCD]].

We suggest to use a fairly recent version, best use the upstream version from
their [git repository](http://sourceforge.net/p/openocd/code/ci/master/tree/).
Version 0.9.0-dev-00148-g3a4ec66 is reported to work.

### Programming the Device
To program the Yunjia NRF51822 module, just go to your RIOT application and
type:
```
make flash
```
and voila, the new firmware should be flashed onto your device.

### Resetting the Device
As the Yunjia module does not provide a reset button, RIOT includes a target
to reset the board. To do that, just type
```
make reset
```
and your board will reboot.

### Debugging the Device
The debugging setup comprises of two parts: a GDB server and a GDB client. To
usual workflow is to start the GDB server first and then connect to it with some
kind of front-end (e.g. command line, IDE, ...).

To start the GDB server, just type
```
make debug-server
```
This will start a local GDB server on `port 3333`.

If you are fine with working with the GDB command line client, you can start
debugging your device by just typing
```
make debug
```
in a second terminal window. This will automatically connect to your
previously opened GDB server and will also load your corresponding .elf file.

Alternatively you can configure your IDE (e.g. eclipse or similar) to connect
directly to the GDB server.
[See here for more information on how to configureEclipse](https://github.com/RIOT-OS/RIOT/wiki/Using-the-Eclipse-IDE-for-C-and-CPP-Developers,-Howto)


### Program the device manually
For OpenOCD to work correctly, you need the following configuration file
(which you can also find in `RIOTDIR/boards/yunjia-nrf51822/dist/openocd.cfg`:

```
 $ cat RIOTDIR/boards/yunjia-nrf51822/openocd.cfg
# nRF51822 Target
source [find interface/stlink-v2.cfg]

transport select hla_swd

set WORKAREASIZE 0x4000
source [find target/nrf51.cfg]

# use hardware reset, connect under reset
#reset_config srst_only srst_nogate
```

You can now program your device by doing the following:

1. start openocd with: `openocd -d3 -f RIOTDIR/boards/yunjia-
nrf51822/dist/openocd.cfg`
2. open a new terminal an connect with telnet: `telnet 127.0.0.1 4444`
3. do the following steps to flash (only use bank #0 starting from address
0):

```
> flash banks
#0 : nrf51.flash (nrf51) at 0x00000000, size 0x00040000, buswidth 1,
chipwidth 1
#1 : nrf51.uicr (nrf51) at 0x10001000, size 0x000000fc, buswidth 1, chipwidth
1

> halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0x61000000 pc: 0x00000e1a msp: 0x20001b2c

> flash write_image erase PATH-TO-YOUR-BINARY/YOUR-BINARY.bin 0
wrote xxx bytes from file PATH-TO-YOUR-BINARY/YOUR-BINARY.bin in xx.yys
(x.yyy KiB/s)

> reset
```

### Debugging manually

First you have to start OpenOCD as described in the section above.

Then enter the following in a new terminal:

```
$ arm-none-eabi-gdb -tui "<your binary ELF>"

(gdb) target remote localhost:3333
Remote debugging using localhost:3333
0x000119ce in ?? ()

(gdb) load
Loading section .text, size 0x2284 lma 0x16000
Loading section .ARM.exidx, size 0x8 lma 0x18284
Loading section .data, size 0x458 lma 0x1828c
Loading section .jcr, size 0x4 lma 0x186e4
Transfer rate: x KB/sec, xxxx bytes/write.

(gdb) monitor reset halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0xc1000000 pc: 0x000006d0 msp: 0x000007c0

(gdb) break main
Breakpoint 3 at 0x123123: file ../main.c, line xx.

(gdb) continue
Continuing.
```

### Using UART

The UART pins are configured in [`boards/yunjia-nrf51822/include/periph_conf.h`](https://github.com/RIOT-OS/RIOT/blob/master/boards/yunjia-nrf51822/include/periph_conf.h).
The default values are PIN 1: `UART_RX` and Pin 2: `UART_TX` _(also compare
schematic above)_.

The BLE400 development board contains a CP2102 accessible through
/dev/tty.SLAB_USBtoUART. You have to edit periph_conf.h to use it with UART_RX
pin 11 and UART_TX pin 9.

The default Baud rate is `115 200`.

### Troubleshooting
#### Protected at factory (error writing to flash at address 0x000... )

This step may be executed before the first programming of the board. OpenOCD
provides the "nrf51 mass_erase" command to remove the protected flag on the
boot-loader region. RIOT does not use the proprietary Nordic "soft-device". This
is how to run the mass erase:

1. Open two terminal windows.
2. In one window, go to the RIOT root folder and run the regarding OpenOCD
configuration script with: `openocd -f boards/yunjia-nrf51822/dist/openocd.cfg`.
3. In the second terminal, connect to the device via telnet: `telnet
127.0.0.1 4444`.
4. If you are successfully connected via telnet, first type `halt` and then
`nrf51 mass_erase`.

This should have erased the whole memory of the device.
