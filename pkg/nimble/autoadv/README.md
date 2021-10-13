# NimBLE automated advertisements

Module for automated bluetooth advertising. Advertising is restarted on
disconnect events automatically. Defaults to the following characteristics:
    - General discoverable mode (BLE_GAP_DISC_MODE_GEN)
    - Undirected connectable mode (BLE_GAP_CONN_MODE_UND)
    - No expiration (BLE_HS_FOREVER)
    - No name

It can be enabled by adding
```
USEMODULE += nimble_autoadv
```
to your makefile.

If your application is calling functions from nimble, e.g.
ble_svc_gap_device_name_set(), NIMBLE_AUTOADV_START_MANUALLY should be set to 1
with the following line in your Makefile:
```
CFLAGS += -DNIMBLE_AUTOADV_START_MANUALLY=1
```
Then the application should call nimble_autoadv_adv_start() after all of its
nimble calls to prevent errors like BLE_HS_EBUSY.

To specify a device name add the following line to your Makefile:
```
CFLAGS += -DNIMBLE_AUTOADV_DEVICE_NAME='"Riot OS device"'
```

By the default, in the advertised packet, the module includes the advertising
**Flags** data type. According to Bluetooth Core Specification Supplement (see
[§1.3.1](https://www.bluetooth.com/specifications/specs/core-specification-supplement-9/))
> The Flags data type shall be included when any of the Flag bits are non-zero
and the advertising packet is connectable, otherwise the Flags data type may be
omitted.

If your application is not connectable (eg. a temperature sensor advertising
its current value), you might want omit this flag by clearing the
`NIMBLE_AUTOADV_FLAG_FIELD` when including this module:
```
CFLAGS += -DNIMBLE_AUTOADV_FLAG_FIELD=0
```
This will grant three extra bytes in the advertisement packet.
