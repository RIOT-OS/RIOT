# BLE CLA example

Note: For an initial understanding of bplib and DTN, it might be helpful to
look at the [UDP + native](../bplib_cla_udp/README.md) example first.

### 2 devices

Using the BLE L2CAP CLA, each node is either a client or a server. A client connects
to the server by a fixed MAC address currently and a server accepts any connection.

To test this, flash it once for the client and once for the
server. This can be selected using the `ROLE` flag. For the client, the remote
MAC address to connect to should also be set with the `REMOTE` flag.

So either `make all ROLE=client REMOTE=d8:6b:d5:e9:ca:99` or `make all ROLE=server`.

When both devices are running, bundles can be exchanged by using the `bp` shell command.

In this version no storage is used (`bplib_stor_void`) and bundles that cannot be
delivered immediately will be dropped. <br>
This can be changed in the Makefile by selecting a different storage
module (`bplib_stor_vfs_ordered` or `bplib_stor_vfs_unordered`), a vfs implementation.
Refer to the UDPCL example Makefile for more details, of how to set the path under which
bundles are saved.
