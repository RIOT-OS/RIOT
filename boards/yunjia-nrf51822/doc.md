@defgroup    boards_yunjia-nrf51822 Yunjia NRF51822
@ingroup     boards
@brief       Support for the Yunjia NRF51822 board

## Overview

The NRF51822 module by Yunjia Technology is a minimalistic but very low-
priced module utilizing Nordics NRF51822QFAA SoC. The SoC features 16Kb of RAM,
256Kb of flash ROM and apart of the usual microcontroller peripherals
with a 2.4GHz radio that supports both Nordics proprietary ShockBurst as well as
Bluetooth Low Energy (BLE).

The board is available for example on ebay or Aliexpress for around 8-10 USD.

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

## Flashing and Debugging

The Yunjia NRF51822 module comes without any on-board programming and
flashing capabilities. It can be programmed using of-the-shelf
programmers such as Segger's JLink or STM's STLink.

A very simple and affordable way to program and debug the Yunjia module is to use
the integrated ST-Link/V2 programmer of most STM32 development board. The required
steps are described in the following sections. If you want to use a standalone
ST-Link adapter, you just simply have to alter the wiring to fit for your
programmer, the software part is identical.

### Hardware

If you are using an STM32 discovery board, you have to enable the standalone ST-Link mode
by removing the two `CN2` jumpers, found somewhere in the upper right part of
the board. This disconnects the ST-Link programmer from the microcontroller
part of the port and enables direct access through the pin-header `CN3`, also
labeled `SWD`.

The Yunjia NRF51822 module supports the Serial Wire Debug (SWD) interface. To
access the device the following four lines need to be connected with the
STM32x-discovery board:

```text
                 Yunjia module    STM32Fx-discovery
common ground:       GND <-----------> GND
supply voltage:      VDD <-----------> 3V
SWD clock:           SWD <-----------> SWCLK (CN3, pin2)
SWD data I/O:      SWDIO <-----------> SWDIO (CN3, pin4)
```

The following image shows the wiring for an STM32F0-discovery board (although
not the most beautiful...):

![Connecting the Yunjia module to an STM32F0-discoveryboard](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/yunjia-wiring.jpg)

### Programming the Device

RIOT's provided functionality for debugging and flashing the Yunjia NRF51822 is
based on the [Open On-Chip Debugger](http://openocd.org/) tool. A guide with
installation instructions and some other information can be found
[here](https://doc.riot-os.org/misc/openocd/).

You can flash the board by calling the following command:

```shell
BOARD=yunjia-nrf51822 make flash
```

and voila, the new firmware should be flashed onto your device.

### Resetting the Device

As the Yunjia module does not provide a reset button, RIOT includes a target
to reset the board. To do that, just type

```shell
BOARD=yunjia-nrf51822 make reset
```

and your board will reboot.

### Debugging the Device

The debugging setup comprises of two parts: a GDB server and a GDB client. To
usual workflow is to start the GDB server first and then connect to it with some
kind of front-end (e.g. command line, IDE, ...).

To start the GDB server, just type

```shell
BOARD=yunjia-nrf51822 make debug-server
```

This will start a local GDB server on `port 3333`.

If you are fine with working with the GDB command line client, you can start
debugging your device by just typing

```shell
BOARD=yunjia-nrf51822 make debug
```

in a second terminal window. This will automatically connect to your
previously opened GDB server and will also load your corresponding .elf file.

Alternatively you can configure your IDE (e.g. eclipse or similar) to connect
directly to the GDB server.
[See here for more information on how to configureEclipse](https://github.com/RIOT-OS/RIOT/wiki/Using-the-Eclipse-IDE-for-C-and-CPP-Developers,-Howto)

### Using UART

The default values are PIN 1: `UART_RX` and Pin 2: `UART_TX` _(also compare
schematic above)_.

The BLE400 development board contains a CP2102 accessible through
/dev/tty.SLAB_USBtoUART. You have to edit periph_conf.h to use it with UART_RX
pin 11 and UART_TX pin 9.

The default Baud rate is `115200`.

### Troubleshooting

#### Protected at factory (error writing to flash at address 0x000... )

This step may be executed before the first programming of the board. OpenOCD
provides the "nrf51 mass_erase" command to remove the protected flag on the
bootloader region. RIOT does not use the proprietary Nordic "soft-device". This
is how to run the mass erase:

1. Open two terminal windows.
2. In one window, go to the RIOT root folder and run the regarding OpenOCD
configuration script with: `openocd -f boards/yunjia-nrf51822/dist/openocd.cfg`.
3. In the second terminal, connect to the device via telnet: `telnet
127.0.0.1 4444`.
4. If you are successfully connected via telnet, first type `halt` and then
`nrf51 mass_erase`.

This should have erased the whole memory of the device.
