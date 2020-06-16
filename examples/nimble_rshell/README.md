NimBLE Remote Shell Example
==========================
This example application configures the NimBLE BLE stack to run a Nordic UART
Service hosting a shell.

It creates one service with two characteristics - one for UART TX and one for
RX. When the shell has data to send, the TX characteristic sends a notification
to the remote host. When the remote host has data for the shell to receive,
the remote host writes to the RX characteristic.

These characteristics can be explored with Nordic's `nRF Connect` phone app.

There are many applications that support this UART service. For Linux there is
`ble-serial` which creates a /dev/ttyBLE that `picocom` can connect to. There are
also phone apps such as `Serial Bluetooth Terminal` for Android.
