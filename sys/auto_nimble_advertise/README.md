# Auto NimBLE advertise

This module is used for automated bluetooth advertising.
It will take care of enabling advertising on disconnect events and
disabling advertising on connect events. It can be enabled by adding
```
USEMODULE += auto_nimble_advertise
```
to your makefile.

The advertised device name can then optionally be configured with
```
CFLAGS += -DAUTO_NIMBLE_ADVERTISE_DEVICE_NAME='"name"'
```
Otherwise the device will appear as "*RIOT OS device*".
