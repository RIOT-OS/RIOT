# Overview

This package provides necessary Makefiles and glue code to use the
Nordic S132 SoftDevice as a standalone Bluetooth BLE interface or as a
transport for RIOT's 6lowpan stack.

This version is based on nRF5 SDK version 15.0.0 a53641a

# Usage

There are two ways to use this package: with or without the 6lowpan stack.

For using *with* the 6lowpan stack, the board `nrf52dk` has a dependency for
`gnrc_netdev_default` to include `nordic_softdevice_sixlowpan`, so all
examples automatically download the SDK and compile / link / flash all
needed code, if you compile for `nrf52dk`.

If you compile for some other board that uses nRF52, you have to either

```
   include $(RIOTBOARD)/nrf52dk/Makefile.dep
```

or manually include the Nordic sixlowpan stack:

```
   USEMODULE += nordic_softdevice_sixlowpan
```

There is a dependency from the `nordic_softdevice_sixlowpan` module to
the `nordic_softdevice_ble` package, which implements the Bluetooth
BLE interface.

See `README-BLE-6LoWPAN.md` for instructions on how to set up 6lowpan
over BLE on Linux.

For using *without* the 6lowpan stack, for BLE only, do not use
the `gnrc_netdev_default` or `nordic_softdevice_sixlowpan` modules.
Instead, just use `nordic_softdevice_ble`:

```
   USEPKG += nordic_softdevice_ble
```

By default that compiles the IPSP MAC layer.  Further work is needed
to remove this dependency to reduce the code footprint.

# Using with more than one parallel connections

If you need to support more than one parallel connections, you have to
modify the following lines in `pkg/nordic_softdevice_ble/Makefile.include`:

```
CFLAGS += -DNRF_SDH_BLE_TOTAL_LINK_COUNT=1
CFLAGS += -DNRF_SDH_BLE_PERIPHERAL_LINK_COUNT=1
```

# Using without LF crystal

If your board does not include the LF crystal, you have to define the
following definitions into your Makefile:

```
CFLAGS += -DNRF_SDH_CLOCK_LF_SRC=0
CFLAGS += -DNRF_SDH_CLOCK_LF_RC_CTIV=6
CFLAGS += -DNRF_SDH_CLOCK_LF_RC_TEMP_CTIV=2
CFLAGS += -DNRF_SDH_CLOCK_LF_ACCURACY=1
```

The nRF52DK does have the LF crystal, so with that you don't need to
enable this.

# Issues / to do

Currently the SoftDevice interrupts are ran in the stack of whatever
thread happens to be running, of the the Idle thread.  This requires
that all threads in the app need to have a reasonably large stack.

Using the SoftDevice seems to trigger a latent bug with RIOT threading
code. When the SoftDevice is used, the threads sometimes block at
mutexes. This seem to happen especially when xtimer is used. This
happens pretty rarely, depending on the application. In some
applications it happens every 10 min ... a couple of hours.
