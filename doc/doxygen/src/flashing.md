Flashing via RIOT's Build System                                    {#flashing}
================================

[TOC]

General Approach                                            {#flashing-general}
================

In general, flashing a board from RIOT is as straight forward as typing in a
shell (with the application directory as current working directory):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=<BOARD-TO-FLASH> flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This will **rebuild** ***AND*** **flash** the application in the current working
directory for board `<BOARD-TO-FLASH>`, using its default programming tool. If
you want to use an alternative programming tool, say `stm32flash`, use:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=<BOARD-TO-FLASH> PROGRAMMER=stm32flash flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To flash without rebuilding use `flash-only` as target instead of `flash`.




Compatibility Matrix                                  {#flashing-compatibility}
====================

Note that some programmers require additional configuration on a per board
level or rely on features only available on some boards. Hence, a board `FOO` of
MCU family `BAR` may not be supported by programmer `BAZ`, even though `BAZ` is
listed as supported for MCU family `BAR`.

This table will use the value to pass to `PROGRAMMER=<value>` as title, rather
than the official spelling of the programmer.

MCU Family     | `adafruit-nrfutil` | `avrdude` | `bmp` | `bossa` | `cc2538-bsl`| `cpy2remed` | `dfu-util` | `edbg` | `elf2uf2` | `esptool` | `goodfet` | `jlink` | `lpc2k_pgm` | `mspdebug` | `nrfutil` | `openocd` | `pic32prog` | `pyocd` | `robotis-loader` | `stm32flash` | `stm32loader` | `uf2conv` | `uniflash`
---------------|--------------------|-----------|-------|---------|-------------|-------------|------------|--------|-----------|-----------|-----------|---------|-------------|------------|-----------|-----------|-------------|---------|------------------|--------------|---------------|-----------|-----------
ATmega         | ✗                  | ✓         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✗       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
ATXmega        | ✗                  | ✓         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✗       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
CC2538         | ✗                  | ✗         | ✗     | ✗       | ✓           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
CC13xx / C26xx | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓ (3)     | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✓
EFM32          | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
ESP8266        | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✓         | ✗         | ✗       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
ESP32 (Xtensa) | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✓         | ✗         | ✗       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
ESP32 (RISC-V) | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✓         | ✗         | ✗       | ✗           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
FE310          | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
GD32V          | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✗       | ✗           | ✗          | ✗         | ✓ (3)     | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
Kinetis        | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
LPC1768        | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
LPC23xx        | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✗       | ✓           | ✗          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
MIPS32r2       | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✗         | ✓           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
MSP430         | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✓         | ✗       | ✗           | ✓          | ✗         | ✗         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
nRF51          | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✓       | ✗                | ✗            | ✗             | ✗         | ✗
nRF52          | ✓ (1)              | ✗         | ✓     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✓ (1)     | ✓         | ✗           | ✓       | ✗                | ✗            | ✗             | ✓         | ✗
RP2040         | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✓         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
SAM            | ✗                  | ✗         | ✗     | ✓ (1)   | ✗           | ✗           | ✗          | ✓      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
Stellaris      | ✗                  | ✗         | ✗     | ✗       | ✗           | ✗           | ✗          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✗                | ✗            | ✗             | ✗         | ✗
STM32          | ✗                  | ✗         | ✓     | ✗       | ✗           | ✓           | ✓          | ✗      | ✗         | ✗         | ✗         | ✓       | ✗           | ✗          | ✗         | ✓         | ✗           | ✗       | ✓ (1)            | ✓            | ✓             | ✗         | ✗


Remarks:

1. Requires a bootloader to be flashed (rather than a bootloader in ROM)
2. Requires specific hardware, e.g. an embedded programmer
3. Requires a patched version of the programmer tool

Programmer Configuration                              {#flashing-configuration}
========================

This section will list additional configuration options to control the behavior
of a programming tool, such as selecting the hardware adapter used for
programming.

OpenOCD Configuration                         {#flashing-configuration-openocd}
---------------------

### `OPENOCD_DEBUG_ADAPTER`

`OPENOCD_DEBUG_ADAPTER` can be set via command line or as environment variable
to use non-default flashing hardware.

### `OPENOCD_RESET_USE_CONNECT_ASSERT_SRST`

`OPENOCD_RESET_USE_CONNECT_ASSERT_SRST` can be set via command line or as
environment variable to `0` to disable resetting the board via the `SRST` line.
This is useful when the `SRST` signal is not connected to the debug adapter or
when using cheap ST-Link V2 clones with broken `SRST` output. Note that it may
not be possible to attach the debugger while the MCU is in deep sleeping mode.
If this is set to `0` by the user, the user may need a carefully timed reset
button press to be able to flash the board.

### `OPENOCD_PRE_FLASH_CMDS`

`OPENOCD_PRE_FLASH_CMDS` can be set as environment variable to pass additional
commands to OpenOCD prior to flashing, e.g. to disable flash write protection.

### `OPENOCD_PRE_VERIFY_CMDS`

`OPENOCD_PRE_VERIFY_CMDS` can be set as environment variable to pass additional
flags to OpenOCD prior to verifying the flashed firmware. E.g. this is used
in the `pba-d-01-kw2x` to disable the watchdog to prevent it from disrupting
the verification process.

### `OPENOCD_PRE_FLASH_CHECK_SCRIPT`

`OPENOCD_PRE_FLASH_CHECK_SCRIPT` can be set via command line or as
environment variable to execute a script before OpenOCD starts flashing. It is
used for Kinetis boards to prevent bricking a board by locking the flash via
magic value in the flash configuration field protection bits.

The script is expected to exit with code `0` if flashing should resume, or with
exit code `1` if flashing should be aborted.

### `OPENOCD_CONFIG`

`OPENOCD_DEBUG_ADAPTER` can be set via command line or as environment variable
to use non-default OpenOCD configuration file.

### `OPENOCD_TRANSPORT`

`OPENOCD_TRANSPORT` can be set via command line or as environment variable to
select a non-default transport protocol. E.g. to use JTAG rather than SWD for a
board that defaults to SWD use:

```
make PROGRAMMER=openocd OPENOCD_TRANSPORT=jtag
```

Note that OpenOCD configuration file of a given board may only support SWD or
JTAG. Also JTAG requires more signal lines to be connected compared to SWD and
some internal programmers only have the SWD signal lines connected, so that
JTAG will not be possible.
