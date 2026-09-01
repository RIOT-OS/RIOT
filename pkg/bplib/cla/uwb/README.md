## UWB CLA

This implements a CLA to transport bundles of arbitrary size, in principle only
limited by the transmission buffers, over UWB using decadriver.

Since UWB again is just a PHY of 802.15.4, this could be extended to be used as
general CLA over 802.15.4 on one of the available layers (submac, netdev, ...).

## Specification

This is a rough specification of a CLA to transport DTN bundles over 802.15.4
frames. It is very experimental.

This does not target the discovery and setup of the 802.15.4 connection. The
link shall already be established, no other protocol (like 6LoWPAN) can be used
simultaneously, i.e. both parties know they use this CLA protocol.

The sender, for every bundle transmission, has to increment the sequence number.

### Non-Fragmentation

Bundles that are small enough to fit in one MAC frame including the header can
be appended after the Seq. No.

FRG must not be set. FED shall be ignored.

ACK requests combined with retransmissions are recommended but not necessary.

### Fragmentation

To allow the transmission of larger bundles over underlying layers the
transmission can be fragmented and reassembled by the CLA.

A fragmentation is indicated by the FRG bit. All frames are sent successively,
in order, and each new frame can only be sent if the previous frame has been
ACKd. A fragmented transmission is then terminated by the FED bit.

This implies that at least all fragmented transmissions are sent with ACK
requests and fail if no such ACK is received after a certain amount of retries
(possibly 0).

Not ACKd fragments (after retries) shall stop the transmission of the whole
bundle. New and different bundle transmissions shall be detected by a change in
the sequence number (Seq. No.).

This allows, in principle, fragmentation of unlimited size, only restricted by
the physical size of the send and receive buffers.

### Potential issues

- This design may lead to duplicate reception of fragments when the ACK is lost,
since no fragment number is used. In most cases the CBOR will become invalid and
no bundle is ingressed. Using CRCs for all bundle blocks is still recommended.

  Prefer reducing the bundles size itself (or increasing frame size), so that
fragmentation is not needed.

### Visual
```
 0                               1
 0 1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A C C D E F
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| | |V|F|F|     |               |                               |
|0|0|R|R|E|     |    Seq. No.   |           data...             |
| | |S|G|D|     |               |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

```

- Seq. No.: [8 Bit]

    Sequence number of bundle transmission. Identical for all fragments.


#### Bits

- Bits 0 and 1 are set to 0 to avoid confusion with LoWPAN

- VRS: Version

    Included because why not, maybe someone wants to use and update this
    protocol in the future (and there are bits left).
    Currently only 0 is valid here and corresponds to this document.

- FRG: Is Fragment

    Set IFF the transmission is a fragment and requires reassmebly

- FED: Fragment End

    Set to indicate the end of a fragmented transmission

Bits not mentioned here are unused.
