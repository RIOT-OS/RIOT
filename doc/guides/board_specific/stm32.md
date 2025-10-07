---
title: STM32 Flashing
description: Guide on how to flash STM32 boards
---

## Flashing the Board Using OpenOCD

The ST Nucleo32, 64 and 144 boards include an on-board ST-LINK programmer
and can be flashed using OpenOCD (use version 0.11.0 at least).
OpenOCD is the standard programmer for all Nucleo boards, so no explicit
`PROGRAMMER` environment variable has to be set.

To flash this board, just use the following command (replace the `xxxxx`
with your Nucleo variant):

```shell
make BOARD=nucleo-xxxxx flash -C examples/basic/hello-world
```

If your board does not have OpenOCD set as the default programmer, you can
select it by explicitly setting the `PROGRAMMER` variable:

```shell
make BOARD=xxxxx PROGARMMER=openocd flash
```

## Flashing the Board Using the ST-LINK Mass Storage Device

The on-board ST-Link programmer found on all Nucleo32, 64 and 144 boards
will show up as a mass storage device when plugged in via USB.
Copying a HEX file to the mass storage device will trigger the flashing
sequence of the ST-Link. This can either be done manually or with the
`cpy2remed` (copy to removable media) PROGRAMMER script. To use this programmer,
you can use the following command:

```shell
make BOARD=nucleo-xxxx PROGRAMMER=cpy2remed flash
```

:::note
If the flash operation fails for some reason, it is possible that
the embedded ST-Link firmware is either too old or has bugs that have been
fixed in the meantime. You can find updates for the ST-Link on
[this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).
:::

## Flashing the Board using stm32flash

It is possible to automatically boot the STM32 board into the in-ROM bootloader
that `stm32flash` communicates with for flashing by connecting the RST pin to
DTR and the BOOT pin (or BOOT0 for STM32 MCU families with BOOT0 and BOOT1 pins)
to RTS of the TTL adapter. In addition, set `STM32FLASH_RESET` to `1` via
environment or command line to actually issue a reset with BOOT (or BOOT0)
pulled high prior flashing to enter the bootloader, and a second reset with BOOT
(or BOOT0) pulled low to reboot into the application. `STM32FLASH_RESET`
defaults to `0` as of know, as with `PROGRAMMER=stm32flash STM32FLASH_RESET=1`
additional terminal flags are set, so that `make term` doesn't accidentally
keep the reset signal pulled low or boots the board into the bootloader.

```shell
make BOARD=xxxx PROGRAMMER=stm32flash STM32FLASH_RESET=1 flash
```

The TTL adapter this was tested with had inverted RTS and DTR signal. By setting
`STM32FLASH_RESET_INVERT` to `1` RIOT will assume RTS and DTR signals to be
inverted, by setting it to `0` non-inverted signals will be generated. As of
now, `STM32FLASH_RESET_INVERT` is by default `1`. This may change if it
becomes evident that non-inverted TTL adapters are in fact more common than
inverted adapters.
