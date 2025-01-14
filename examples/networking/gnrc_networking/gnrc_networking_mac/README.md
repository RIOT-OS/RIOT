# gnrc_networking_mac example

This example shows you how to try out communications between RIOT instances with duty-cycled MAC layer protocols (GoMacH and LWMAC) for IEEE 802.15.4 devices.
This example is generally based on `gnrc_networking` but embeds GoMacH (or LWMAC) to support low duty-cycle operation to conserve power. Also, it intends to show that the duty-cycled MAC protocol can support popular upper layer protocols like UDP and RPL.
Currently, it seems that you can only use samr21-xpro and iotlab-m3 boards (in case of using LWMAC, then only samr21-xpro board) to test with this example, since some certain features of the MAC protocol are only available on that platform. Also, the current implementations of GoMacH and LWMAC use RTT as the underlying timer source. So, currently, GoMacH and LWMAC cannot run on nodes that don't have RTT. But, as a long-term plan, we will replace RTT by a general timer API as the underlying timer to make GoMacH and LWMAC available for more devices, when the related implementations are ready.


## Usage

Build, flash and start the application:
```
export BOARD=your_board
make
make flash
make term
```

## Print out the achieved duty-cyle of GoMacH

You can print out the radio duty-cyle (a roughly one) of GoMacH by setting the `GNRC_GOMACH_ENABLE_DUTYCYLE_RECORD` flag in `sys/include/net/gnrc/gomach/types.h` to "1". By doing so, each time when a device sends or receives a packet, it will print out its achieved radio duty-cycle value.

## Print out the achieved duty-cyle of LWMAC

You can print out the radio duty-cyle (a roughly one) of LWMAC by setting the `LWMAC_ENABLE_DUTYCYLE_RECORD` flag in `sys/include/net/gnrc/lwmac/types.h` to "1". By doing so, each time when a device sends or receives a packet, it will print out its achieved radio duty-cycle value.
Also, by further enabling the debug flag in `sys/net/gnrc/link_layer/lwmac/tx_state_machine.c`, you will get the printout of how many preamble (WR) and time (sending delay) cost for sending this packet in the TX procedure of LWMAC.

## Try UDP transmissions with LWMAC (or GoMacH)

In the RIOT shell, get to know the IP address of one node:

2017-06-06 15:05:48,279 - INFO # ifconfig
2017-06-06 15:05:48,284 - INFO # Iface  7   HWaddr: 79:f6  Channel: 26  Page: 0  NID: 0x23
2017-06-06 15:05:48,288 - INFO #            Long HWaddr: 79:67:35:7e:54:3a:79:f6
2017-06-06 15:05:48,297 - INFO #            TX-Power: 0dBm  State: SLEEP  max. Retrans.: 3  CSMA Retries: 4
2017-06-06 15:05:48,303 - INFO #            CSMA  MTU:1280  HL:64  6LO  RTR  IPHC
2017-06-06 15:05:48,306 - INFO #            Source address length: 8
2017-06-06 15:05:48,309 - INFO #            Link type: wireless
2017-06-06 15:05:48,314 - INFO #            inet6 addr: ff02::1/128  scope: local [multicast]
2017-06-06 15:05:48,320 - INFO #            inet6 addr: fe80::7b67:357e:543a:79f6/64  scope: local
2017-06-06 15:05:48,326 - INFO #            inet6 addr: ff02::1:ff3a:79f6/128  scope: local [multicast]
2017-06-06 15:05:48,331 - INFO #            inet6 addr: ff02::1a/128  scope: local [multicast]

and start a UDP server.

    > udp server start 8808

This node is now ready to receive data on port `8808`.

In a second terminal, start a second RIOT instance, in the RIOT shell, you can now send a message to the first RIOT instance:

    > udp send fe80::7b67:357e:543a:79f6 8808 testmessage

In your first terminal (the receiver side), you should now see output that looks like this:

2017-06-06 15:00:06,894 - INFO # [LWMAC]: achieved duty-cycle: 10 %
2017-06-06 15:00:06,896 - INFO # PKTDUMP: data received:
2017-06-06 15:00:06,901 - INFO # ~~ SNIP  0 - size:  11 byte, type: NETTYPE_UNDEF (0)
2017-06-06 15:00:06,907 - INFO # 00000000  74  65  73  74  6D  65  73  73  61  67  65
2017-06-06 15:00:06,911 - INFO # ~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)
2017-06-06 15:00:06,914 - INFO #    src-port:  8808  dst-port:  8808
2017-06-06 15:00:06,917 - INFO #    length: 19  cksum: 0xf729
2017-06-06 15:00:06,921 - INFO # ~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (3)
2017-06-06 15:00:06,925 - INFO # traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)
2017-06-06 15:00:06,927 - INFO # flow label: 0x00000
2017-06-06 15:00:06,930 - INFO # length: 19  next header: 17  hop limit: 64
2017-06-06 15:00:06,934 - INFO # source address: fe80::7b67:877:19f:331e
2017-06-06 15:00:06,938 - INFO # destination address: fe80::7b67:357e:543a:79f6
2017-06-06 15:00:06,943 - INFO # ~~ SNIP  3 - size:  24 byte, type: NETTYPE_NETIF (-1)
2017-06-06 15:00:06,945 - INFO # if_pid: 7  rssi: 51  lqi: 255
2017-06-06 15:00:06,946 - INFO # flags: 0x0
2017-06-06 15:00:06,949 - INFO # src_l2addr: 79:67:08:77:01:9f:33:1e
2017-06-06 15:00:06,952 - INFO # dst_l2addr: 79:67:35:7e:54:3a:79:f6
2017-06-06 15:00:06,956 - INFO # ~~ PKT    -  4 snips, total size:  83 byte


In your second terminal (the sender side), you should now see output that looks like this:

2017-06-06 15:00:06,871 - INFO # udp send fe80::7b67:357e:543a:79f6 8808 testmessage
2017-06-06 15:00:06,877 - INFO # Success: sent 11 byte(s) to [fe80::7b67:357e:543a:79f6]:8808
2017-06-06 15:00:06,890 - INFO # [LWMAC-tx]: spent 1 WR in TX
2017-06-06 15:00:06,894 - INFO # [LWMAC-tx]: pkt sending delay in TX: 8422 us
2017-06-06 15:00:06,898 - INFO # [LWMAC]: achieved duty-cycle: 10 %
