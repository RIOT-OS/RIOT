# Overview

This package provides necessary Makefiles and glue code to use the Nordic S132
SoftDevice as Bluetooth BLE transport for RIOT's 6lowpan stack.

# Usage

"gnrc_netdev_default" has a dependency to "nordic_softdevice_ble", so all
examples automatically download the SDK and compile / link / flash all needed
code.

If you want to manually set up included modules, add "USEPKG +=
nordic_softdevice_ble" to your application's Makefile.

See README-BLE-6LoWPAN.md for instructions on how to set up 6lowpan over BLE on
Linux.

# Known issues

- floating point ABI mismatch

The Nordic binaries are compiled using hard float ABI, which RIOT currently
doesn't support (RIOT's context switching code doesn't preserve the floating
point registers).  If the softdevice is compiled in, RIOT will be compiled with
hard float ABI in order for the linking to succeed.  This works for some
application, but will lead to serious and hard-to-find bugs for code using
floating point instructions.

Hopefully, either the RIOT community adds hard-float support, or Nordic
releases a soft-float version of the used binaries.
