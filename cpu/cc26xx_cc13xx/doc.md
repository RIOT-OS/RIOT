@defgroup       cpu_cc26xx_cc13xx CC26xx/CC13xx common
@ingroup        cpu
@brief          Common code for TI cc26xx/cc13xx family

This module contains code common to all cc26xx/cc13xx cpus
supported by RIOT: @ref cpu_cc26x0_cc13x0, @ref cpu_cc26x2_cc13x2

\section cc26xx_cc13xx_riot RIOT-OS on CC26xx/CC13xx boards

# Overview

The CC26xx/C13xx is a family of micro controllers fabricated by Texas Instruments
for low-power communications, using protocols such as BLE, IEEE 802.15.4g-2012,
and proprietary radio protocols.

These family of MCUs is divided in two generations, the cc26x0/cc13x0, and the
cc26x2/cc13x2 family. The difference is that the later provides more ROM and RAM
and improvements on various peripherals.

 MCU family    | RAM  | Flash
:--------------|:-----|:------
 CC26x0/CC13x0 | 20 K | 128 K
 CC26x2/CC13x2 | 80 K | 352 K

@note The actual flash size is the flash size minus 88 bytes, these 88 bytes are
reserved for the CCFG, see also [Flashing the CCFG](#cc26xx_cc13xx_ccfg).

# Flashing the CCFG

@warning Setting an incorrect CCFG configuration may lock out yourself
out of the device.

@note Blank chips from Texas Instruments come without a CCFG flashed, so any
firmware flashed won't boot until the configuration is flashed. As this might be
the case for custom boards remember flashing it.

RIOT provides built-in support to flash the Customer Configuration on the
CC26xx/CC13xx MCUs. It can be done through Kconfig using `make menuconfig`.

For example:

```
make -C examples/basic/hello-world menuconfig BOARD=cc1350-launchpad
```

It will open the Kconfig terminal configuration utility, you may see the
`Update CCFG` option, selecting it will include the default configuration that
Texas Instruments provides from their own SDK. You may change any further
options available through Kconfig.

Once configuration is saved you may compile a new binary and flash it onto the
device.

For example:

```
make -C examples/basic/hello-world flash BOARD=cc1350-launchpad
```

@note Once flashed, there's no need to flash it again, unless the configuration
needs to be changed.

# Debugging

Development kits from Texas Instruments come with an XDS110 on-board debug probe
that provides programming, flashing and debugging capabilities.

It can either use proprietary Texas Instruments tools for programming, or OpenOCD.

### Using Upstream OpenOCD

OpenOCD is the default programmer and debugger. Hence, flashing can be done
by navigating to the application directory and running:

```
make flash BOARD=<CC26xx-or-CC13xx-based-board>
```

@warning    A reliable and robust sequence to reset the CC26xx / CC13xx from
            upstream OpenOCD is not yet implemented. As a result, OpenOCD will
            halt the MCU for flashing without reset. This may result in IRQ
            handlers being already set up and responding to IRQs that trigger
            while flashing. Hence, flashing is likely not 100% reliable.

@note       By default the XDS110 debug adapter is used, which is the debugger
            TI integrates into is developments boards. This can be overwritten
            by setting `OPENOCD_DEBUG_ADAPTER` to a different debugger.

### Using TI's OpenOCD Fork

TI maintains an outdated fork of OpenOCD that contains patches and special
handling that have not upstreamed yet. It can be build using:

```
# Clone into the openocd-ti folder
git clone https://git.ti.com/cgit/sdo-emu/openocd openocd-ti

# Change directory to the openocd source code
cd openocd-ti/openocd

# Configure, build, install
./configure
make
sudo make install
```

@warning    Sometimes OpenOCD may stop working when the firmware on the XDS110
            is updated (when using Uniflash, happens without user intervention).
            With that in mind, it's encouraged to either enable the ROM
            bootloader backdoor to enable serial programming or the
            installation of TI Uniflash as a fallback. See
            [Using Uniflash](#cc26xx_cc13xx_uniflash)

@note       With `OPENOCD_CMD_RESET_HALT="-c 'reset halt'"` the default reset
            sequence can be restored. This may work with TI's OpenOCD fork.

Otherwise, usage is identical with the upstream version of OpenOCD.

### using J-Link

By passing (or exporting) `PROGRAMMER=jlink` J-Link can be used to flash the
board. This requires a J-Link compatible programmer / debugger. Since the
XDS110 that TI's development boards use is not compatible, an external
programmer has to be used. The upside is that flashing appears to be reliable
with that.

### Using Uniflash

The TI's Code Composer Studio provides the necessary tools to use the debug
features of the XDS110; Uniflash provides flashing tools. Both programs can
be found here:

- [Code Composer Studio (CCS) Integrated Development Environment (IDE)](http://www.ti.com/tool/CCSTUDIO).
- [Uniflash Standalone Flash Tool for TI Microcontrollers (MCU), Sitara Processors & SimpleLink devices](http://www.ti.com/tool/UNIFLASH).

Before using the XDS110 with the latest CCS/Uniflash versions the firmware for
it needs to be updated. Texas Instruments has a guide to correctly update it
[here](http://software-dl.ti.com/ccs/esd/documents/xdsdebugprobes/emu_xds110.html#updating-the-xds110-firmware).

#### Setting up the environment

In order to make use of the programming and debugging capabilities of the XDS110
some environment variable needs to be set:

```
export CCS_PATH=<path to ti install folder>/ti/ccs930
export UNIFLASH_PATH<path to ti install folder>/ti/uniflash_5.2.0
```

That assumes you have CCS 9.3.0 (for the path name) and Uniflash 5.2.0, adjust
accordingly.

After that you can flash using the RIOT `make flash` command on your application
or to debug you first start the debug server:

```
make debug-server
```

And then on another terminal you can run:

```
make debug
```

It will open GDB and connect to the debug server automatically.

@note By default LaunchPad boards on RIOT use uniflash as the default
programmer, if it's not the case for an external board, you can always use
uniflash by setting this environment variable `PROGRAMMER=uniflash` to change
the default programmer.

@defgroup       cpu_cc26xx_cc13xx_definitions CC26xx/CC13xx common
@ingroup        cpu
@brief          Common definitions for TI cc26xx/cc13xx family

This module contains definitions common to all cc26xx/cc13xx cpus
supported by RIOT: @ref cpu_cc26x0_cc13x0, @ref cpu_cc26x2_cc13x2
