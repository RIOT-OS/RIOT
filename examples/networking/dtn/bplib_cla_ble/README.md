# BLE CLA example

Note: For an initial understanding of bplib and DTN, it might be helpful to
look at the [UDP + native](../bplib_cla_udp/README.md) example first.

### 2 devices

Using the BLE L2CAP CLA, each node is either a client or a server. A client connects
to the server by a fixed MAC address currently and a server accepts any connection.

To test this, copy this example and flash it once for the client and once for the
server (edit the bool in `bplib_cla_ble_start()` and make sure to set the MAC address
of the other server correctly on the client using the `bplib_contact_set_out_addr()`).

When run, both devices should then run and, using the bp command, bundles can be exchanged.

In this version no storage is used and bundles that cannot be delivered immediately will
be dropped.
