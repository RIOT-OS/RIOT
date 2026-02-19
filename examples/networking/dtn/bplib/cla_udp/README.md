# DTN example using bplib

[What is DTN?](https://en.wikipedia.org/wiki/Delay-tolerant_networking) Mainly used in space communication contexts for unreliable and/or very delayed connections.

This port of bplib provides working CLA for UDP for now, which can be run with this example on the `native` board. It should thus also work with some underlying protocol between two physical boards and 802.15.4 or BLE.

The port is still is the works with the interfaces that are provided to the user. For now the local node ID is adjusted in the `Makefile` in the `BPLIB_LOCAL_EID_NODE_NUM` macro.

The remote node ID(s) are adjusted in the `bplib_cla_contactstable`. There you also have to set the IP the UDP CLA will send bundles to.

## Examples
### Loopback

To test simple loopback inside the node set the `BPLIB_LOCAL_EID_NODE_NUM` equal to the `DestEID.Node` in the `bplib_pi_channeltable`.

When using the `bp 0 "[DATA]"` shell command, the data you entered should go though bplib's internal delivery, since the current node is the destination.

### 2 devices (A <-> B)

To test with two devices create two tap devices as described in the gnrc examples. Copy this example and configure each `DestEID.Node` to point to the `BPLIB_LOCAL_EID_NODE_NUM`. Also adjust the IP of each CLA to the respective other device, you can use the commented out shell command in the first line of `main()` to set some static IPs.

When running `bp 0 "[DATA]"` now, the bundle should be delivered to the other instance.

### 3 devices (A <-> B <-> C)

TBD, have to test myself first, but from the logic just expand on the 2 devices. Either have the B node have 2 CLA entries and start both of them at the same time, or:

Send from A to B as in the previous exapmle, stop the CLA, alter the table such that the connection now points to C and start the CLA again. Bundles should be stored using the `vfs` when no link is available and when the link to C is established, all bundles from the storage should be forwarded to B (according to the configuration of the table).

This is the whole point of DTN, to allow store-and-forward transfer, without direct end-to-end paths always available.
