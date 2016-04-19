# `gnrc_ipv6_ext` test

This test sends a packet to itself with extension headers. This is based on gnrc_networking example.

Enable debug output of `gnrc_ipv6.c` before run. When the test is run, it should show the following debug output:

```
ipv6: Received (src = fd01::1, dst = fd01::2, next header = 0, length = 40)
ipv6: forward nh = 0 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 0)
ipv6: forward nh = 43 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 43)
ipv6: waiting for incoming message.
ipv6: GNRC_NETAPI_MSG_TYPE_RCV received
ipv6: Received (src = fd01::1, dst = fd01::3, next header = 0, length = 40)
ipv6: forward nh = 0 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 0)
ipv6: forward nh = 43 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 43)
ipv6: waiting for incoming message.
ipv6: GNRC_NETAPI_MSG_TYPE_RCV received
ipv6: Received (src = fd01::1, dst = fd01::2, next header = 0, length = 40)
ipv6: forward nh = 0 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 0)
ipv6: forward nh = 43 to other threads
ipv6: unable to forward packet as no one is interested in it
ipv6: handle extension header (nh = 43)
ipv6: forward nh = 17 to other threads
ipv6: Send receive command for 0x1060b8 to 5
ipv6: waiting for incoming message.
pkt->users: 0
```

It configures the network interface with addresses fd01::02 and fd01::03. Then it sends a packet to fd01::02 with a routing extension header containing addresses fd01::03 and fd01::02. So the packet should be forwarded from fd01::02 to fd01::03, then to fd01::02 again.

The packet has a Hop-by-Hop extension header that should be ignored.

The test also asserts that the packet is released.
