NimBLE Heart Rate Sensor Example
================================
This example demonstrates how to implement asynchronous data transfer using GATT
notifications by implementing a mock-up BLE heart rate sensor. Once flashed on
a node, you can connect to it using any phone or device capable of using such a
heart rate sensor, e.g. Nordics `nRF Toolbox` for Android
(https://play.google.com/store/apps/details?id=no.nordicsemi.android.nrftoolbox).
With browsers supporting the experimental [Web Bluetooth API](https://webbluetoothcg.github.io/web-bluetooth/)
(currently only the Chromium / Chrome family), this can also be accessed from a
[demo web application](https://webbluetoothcg.github.io/demos/heart-rate-sensor/).

The heart rate sensor is simulated by simply providing an up- and down counter
for its BPM value.

On top of the heart rate service, this example implements the device information
as well as the battery service.
