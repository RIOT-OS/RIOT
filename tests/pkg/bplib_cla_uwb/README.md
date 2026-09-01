# Test for experimental decadriver UWB CLA

Note: For an initial understanding of bplib and DTN, it might be helpful to
look at the [UDP + native](../../../examples/networking/dtn/bplib_cla_udp/README.md)
example first.

This CLA uses 802.15.4 frames and sends them over UWB using decadriver.

By default the standard frame length of 127 bytes is used, meaning bundles that
would otherwise not fit in one frame will be fragmented. The DW3000 modules
allow up to 1023 byte frames, but are operating outside of the 802.15.4
standard.

Currently only 2 devices can talk to each other, when 2 devices try to send to
one receiver, combined with fragmentation, problems can occur.

### 2 devices

Using the UWB CLA, each node is either a client or a server. These roles only
serve as distinction here, so that the local EID and remote EID are set
correctly. Both roles have the same capabilities.

For this test, both devices also use the same short address and PAN, in order to
not have to change the addresses depending on the role.

To run this test, flash it once for the client and once for the server. This can
be selected using the `ROLE` flag, so either `make all ROLE=client` or
`make all ROLE=server`.

When both devices are running, bundles can be exchanged by using the `bp` shell
command. Use `bp bench 0 ...` to test bundles of larger sizes and multiple
ingressed at once.

In this version no storage is used (`bplib_stor_void`) and bundles that cannot be
delivered immediately will be dropped.

This can be changed in the Makefile by selecting a different storage
module (`bplib_stor_vfs_ordered` or `bplib_stor_vfs_unordered`) and selecting a
vfs implementation.
Refer to the UDPCL example Makefile for more details, of how to set the path
under which bundles are saved.
