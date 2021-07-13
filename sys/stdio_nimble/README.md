# STDIO NimBLE

This module uses NimBLE for stdio. The bluetooth characteristic for
stdin is writable and the characteristic for stdout uses the indicate
mechanism to publish the system's output to a connected device. Calls of
*_send_from_ringbuffer* to send data via nimBLE are chained together
using a callback in nimBLE called *_gap_event_cb*.

To use this module, add
```
USEMODULE += stdio_nimble
```
to your makefile.

You can change the default buffer sizes by adding
```
CFLAGS+=-DSTDIN_BUFSIZE=1024
CFLAGS+=-DSTDOUT_BUFSIZE=2048
```
to your makefile.

For automatic bluetooth advertising a module is provided: *auto_nimble_advertise*.
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
