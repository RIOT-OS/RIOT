# Nordic SoftDevice Test Application
The main purpose of this test application is to ensure the inclusion of the
Nordic SoftDevice package in RIOTs build test.

In addition, this example includes a minimal GNRC configuration and the
corresponding shell commands, so it can be used for some simple tests of
network functionality. Please refer to `pkg/nordic_softdevice_ble/README.md`
and `pkg/nordic_softdevice_ble/README-BLE-6LoWPAN.md` for more information on
how to setup an IPv6 connection to your device.

For more features, you can use the SoftDevice with RIOTs `gnrc_networking`
example application. Simply build `gnrc_networking` for a SoftDevice-capable
device, e.g. `USEPKG=nordic_softdevice_ble BOARD=nrf52dk make ...`.
