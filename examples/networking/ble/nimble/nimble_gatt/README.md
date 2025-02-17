NimBLE GATT Server Example
==========================
This example application configures and runs the NimBLE BLE stack as simple GATT
server. It will provide two services, the built-in GAP and the built-in GATT
service. The device will advertise itself by the name `NimBLE on RIOT`.

To interact with the BLE device running this example, use any BLE client that is
able to discover and interact with GATT services.
An easy way to get started is to use a smartphone (supporting at least BT 4.0) with Nordic's `nRF Connect` App.
