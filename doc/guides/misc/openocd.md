---
title: OpenOCD
description: This page describes the process of using OpenOCD for flashing.
---

## Description

[OpenOCD](http://openocd.sourceforge.net/) (the open on-chip debugger) is an
open source tool for debugging and flashing microcontrollers. In RIOT we try
to use this tool for as many platforms as possible to reduce the overhead of
having to keep track of many different (and sometimes proprietary) tools.

## How to Build/Install

### Linux (Ubuntu)

The following steps are for building and installing OpenOCD under Ubuntu.
For other Linux distributions the steps have to slightly altered.

:::note
Ubuntu usually ships very old versions of OpenOCD, which lack support for
modern microcontrollers. In this case, you will have to build and install
OpenOCD yourself. Consider using the `latest` branch instead of a Release,
since the release cycle of OpenOCD is quite slow as well.
:::

```shell
sudo apt install openocd
```

### MacOS

1. Using [homebrew](http://brew.sh/) one can install the most recent version of OpenOCD by:

```shell
brew install [--HEAD] open-ocd
```

1. Test if everything worked out properly:

```shell
openocd --version
```

This should give you the version you have just build, probably something like
`0.12.0-dev-xxxx`. Voila, that's it!

### Compiling OpenOCD

Please refer to the
[OpenOCD documentation](https://github.com/openocd-org/openocd#compiling-openocd)
for details about how to compile OpenOCD for your system.

#### Problems & Fixes

When using OpenOCD on Mac OS and you encounter an error similar to

```text
Error: libusb_claim_interface() failed with LIBUSB_ERROR_ACCESS
Error: unable to open ftdi device with vid 0403, pid 6010, description '*' and serial '*'
```

make sure you are running `make flash` with `sudo` and that the system `ftdi` driver is unloaded:

From [OpenOCD documentation](https://github.com/openocd-org/openocd/blob/master/README.macOS.md):

If you're using a USB adapter and have a driver kext matched to it, you will need to unload it prior to running OpenOCD. E.g. with Apple driver (OS X 10.9 or later) for FTDI run:

```shell
sudo kextunload -b com.apple.driver.AppleUSBFTDI
```

for FTDI vendor driver use:

```shell
sudo kextunload FTDIUSBSerialDriver.kext
```

## Transports and Board Support

To figure out if a board is supported by OpenOCD it's best to a look on the
scripts that come with OpenOCD. On Ubuntu these are put into
`/usr/local/share/openocd/scripts/`. RIOT uses these OpenOCD scripts via its
own Makefiles. You can find them in the
`RIOT/makefiles/tools/openocd-adapters/` directory.

## udev Rules and Unsupported Boards

When using OpenOCD on Linux, your user might not have the appropriate access
rights to access the USB port directly. Furthermore, old versions of OpenOCD might
not have the support for newer programmers such as the ST-Link V3.
You might encounter an error similar to this:

```shell
Error: libusb_open() failed with LIBUSB_ERROR_ACCESS
Error: no device found
Error: unable to open ftdi device with vid 0403, pid 6010, description 'FITECO M3' and serial '*'
in procedure 'init'
```

chances are you have to configure udev rules to work with the device.
When built from source, OpenOCD provides examples in `contrib/99-openocd.rules`. Copy this
file to your udev configuration directory make sure your `vid` and `pid` are covered and
If you have a recent OpenOCD version, you can check if your user is in the `dialout` and/or
`plugdev` group and therefore has the appropriate access rights.

If you have an old OpenOCD version, you have to compile the latest OpenOCD
sources for your system and manually update the
`/etc/udev/rules.d/60-openocd.rules` file with the file provided in the
`contrib` subfolder of the OpenOCD repository. Please refer to the OpenOCD
documentation for further information.
