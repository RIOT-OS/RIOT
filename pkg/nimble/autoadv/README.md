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
