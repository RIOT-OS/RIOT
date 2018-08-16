#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # embUnit tests
    child.expect(r"OK \(\d+ tests\)")
    # output cross-checked hex data with WireShark -> "Import from Hex Dump..."
    # test_netapi_send__raw_unicast_ethernet_packet
    child.expect("Sending data from Ethernet device:")
    child.expect("00000000  3E  E6  B5  0F  19  23  3E  E6  B5  22  FD  0A  FF  FF  41  42")
    child.expect("00000010  43  44  45  46  47  00")
    # test_netapi_send__raw_broadcast_ethernet_packet
    child.expect("Sending data from Ethernet device:")
    child.expect("00000000  FF  FF  FF  FF  FF  FF  3E  E6  B5  22  FD  0A  FF  FF  41  42")
    child.expect("00000010  43  44  45  46  47  00")
    # test_netapi_send__raw_unicast_ieee802154_long_long_packet
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  DC  00  00  00  0B  FD  22  19  0F  B5  E6  3E  0A  FD  22")
    child.expect("00000010  19  0F  B5  E6  3E  31  32  33  41  42  43  44  45  46  47  00")
    # test_netapi_send__raw_unicast_ieee802154_long_short_packet
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  D8  01  00  00  0B  FD  0A  FD  22  19  0F  B5  E6  3E  31")
    child.expect("00000010  32  33  41  42  43  44  45  46  47  00")
    # test_netapi_send__raw_unicast_ieee802154_short_long_packet1
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  9C  02  00  00  0B  FD  22  19  0F  B5  E6  3E  0A  FD  31")
    child.expect("00000010  32  33  41  42  43  44  45  46  47  00")
    # test_netapi_send__raw_unicast_ieee802154_short_long_packet2
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  9C  03  00  00  0B  FD  22  19  0F  B5  E6  3E  0A  FD  31")
    child.expect("00000010  32  33  41  42  43  44  45  46  47  00")
    # test_netapi_send__raw_unicast_ieee802154_short_short_packet
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  98  04  00  00  0B  FD  0A  FD  31  32  33  41  42  43  44")
    child.expect("00000010  45  46  47  00")
    # test_netapi_send__raw_broadcast_ieee802154_long_packet
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  D8  05  00  00  FF  FF  0A  FD  22  19  0F  B5  E6  3E  31")
    child.expect("00000010  32  33  41  42  43  44  45  46  47  00")
    # test_netapi_send__raw_broadcast_ieee802154_short_packet
    child.expect(r"Sending data from IEEE 802\.15\.4 device:")
    child.expect("00000000  41  98  06  00  00  FF  FF  0A  FD  31  32  33  41  42  43  44")
    child.expect("00000010  45  46  47  00")
    # test_netapi_send__ipv6_unicast_ethernet_packet
    child.expect("Sending data from Ethernet device:")
    child.expect("00000000  3E  E6  B5  0F  19  23  3E  E6  B5  22  FD  0A  86  DD  60  00")
    child.expect("00000010  00  00  00  08  3B  40  FE  80  00  00  00  00  00  00  3C  E6")
    child.expect("00000020  B5  FF  FE  22  FD  0A  FE  80  00  00  00  00  00  00  3C  E6")
    child.expect("00000030  B5  FF  FE  0F  19  23  41  42  43  44  45  46  47  00")
    # test_netapi_send__ipv6_multicast_ethernet_packet
    child.expect("Sending data from Ethernet device:")
    child.expect("00000000  33  33  00  00  00  01  3E  E6  B5  22  FD  0A  86  DD  60  00")
    child.expect("00000010  00  00  00  08  3B  40  FE  80  00  00  00  00  00  00  3C  E6")
    child.expect("00000020  B5  FF  FE  22  FD  0A  FF  02  00  00  00  00  00  00  00  00")
    child.expect("00000030  00  00  00  00  00  01  41  42  43  44  45  46  47  00")
    # test_netapi_send__ipv6_unicast_ieee802154_packet
    child.expect("Sending data from IEEE 802.15.4 device:")
    child.expect("00000000  41  DC  07  00  00  0B  FD  22  19  0F  B5  E6  3E  0A  FD  22")
    child.expect("00000010  19  0F  B5  E6  3E  60  00  00  00  00  08  3B  40  FE  80  00")
    child.expect("00000020  00  00  00  00  00  3C  E6  B5  0F  19  22  FD  0A  FE  80  00")
    child.expect("00000030  00  00  00  00  00  3C  E6  B5  FF  FE  0F  19  23  41  42  43")
    child.expect("00000040  44  45  46  47  00")
    # test_netapi_send__ipv6_multicast_ieee802154_packet
    child.expect("Sending data from IEEE 802.15.4 device:")
    child.expect("00000000  41  D8  08  00  00  FF  FF  0A  FD  22  19  0F  B5  E6  3E  60")
    child.expect("00000010  00  00  00  00  08  3B  40  FE  80  00  00  00  00  00  00  3C")
    child.expect("00000020  E6  B5  0F  19  22  FD  0A  FF  02  00  00  00  00  00  00  00")
    child.expect("00000030  00  00  00  00  00  00  01  41  42  43  44  45  46  47  00")
    # test_netapi_recv__empty_ethernet_payload
    child.expect("pktdump dumping Ethernet packet with empty payload")
    child.expect("PKTDUMP: data received:")
    child.expect(r"~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF \(0\)")
    child.expect(r"00000000~~ SNIP  1 - size:  \d+ byte, type: NETTYPE_NETIF \(-1\)")
    child.expect(r"if_pid: \d+  rssi: -?\d+  lqi: \d+")
    child.expect("flags: 0x0")
    child.expect("src_l2addr: 3e:e6:b5:22:fd:0b")
    child.expect("dst_l2addr: 3e:e6:b5:22:fd:0a")
    child.expect("~~ PKT    -  2 snips, total size:  \d+ byte")
    # test_netapi_recv__empty_ieee802154_payload
    child.expect(r"pktdump dumping IEEE 802\.15\.4 packet with empty payload")
    child.expect("PKTDUMP: data received:")
    child.expect(r"~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF \(0\)")
    child.expect(r"00000000~~ SNIP  1 - size:  \d+ byte, type: NETTYPE_NETIF \(-1\)")
    child.expect(r"if_pid: \d+  rssi: -?\d+  lqi: \d+")
    child.expect("flags: 0x0")
    child.expect("src_l2addr: 3e:e6:b5:0f:19:22:fd:0b")
    child.expect("dst_l2addr: 3e:e6:b5:0f:19:22:fd:0a")
    child.expect("~~ PKT    -  2 snips, total size:  \d+ byte")
    # test_netapi_recv__raw_ethernet_payload
    child.expect("pktdump dumping Ethernet packet with payload 12 34 45 56")
    child.expect("PKTDUMP: data received:")
    child.expect(r"~~ SNIP  0 - size:   4 byte, type: NETTYPE_UNDEF \(0\)")
    child.expect("00000000  12  34  45  56")
    child.expect(r"~~ SNIP  1 - size:  \d+ byte, type: NETTYPE_NETIF \(-1\)")
    child.expect(r"if_pid: \d+  rssi: -?\d+  lqi: \d+")
    child.expect("flags: 0x0")
    child.expect("src_l2addr: 3e:e6:b5:22:fd:0b")
    child.expect("dst_l2addr: 3e:e6:b5:22:fd:0a")
    child.expect("~~ PKT    -  2 snips, total size:  \d+ byte")
    # test_netapi_recv__raw_ieee802154_payload
    child.expect(r"pktdump dumping IEEE 802\.15\.4 packet with payload 12 34 45 56")
    child.expect("PKTDUMP: data received:")
    child.expect(r"~~ SNIP  0 - size:   4 byte, type: NETTYPE_UNDEF \(0\)")
    child.expect("00000000  12  34  45  56")
    child.expect(r"~~ SNIP  1 - size:  \d+ byte, type: NETTYPE_NETIF \(-1\)")
    child.expect(r"if_pid: \d+  rssi: -?\d+  lqi: \d+")
    child.expect("flags: 0x0")
    child.expect("src_l2addr: 3e:e6:b5:0f:19:22:fd:0b")
    child.expect("dst_l2addr: 3e:e6:b5:0f:19:22:fd:0a")
    child.expect("~~ PKT    -  2 snips, total size:  \d+ byte")
    # test_netapi_recv__ipv6_ethernet_payload
    child.expect("pktdump dumping IPv6 over Ethernet packet with payload 01")
    child.expect("PKTDUMP: data received:")
    # payload not dumped because not parsed yet, but header looks okay, so let's
    # assume the payload is as well
    child.expect(r"~~ SNIP  0 - size:  41 byte, type: NETTYPE_IPV6 \(2\)")
    child.expect(r"traffic class: 0x00 \(ECN: 0x0, DSCP: 0x00\)")
    child.expect("flow label: 0x00000")
    child.expect("length: 1  next header: 59  hop limit: 64")
    child.expect("source address: fe80::3fe6:b5ff:fe22:fd0a")
    child.expect("destination address: fe80::3fe6:b5ff:fe22:fd0b")
    child.expect(r"~~ SNIP  1 - size:  \d+ byte, type: NETTYPE_NETIF \(-1\)")
    child.expect(r"if_pid: \d+  rssi: -?\d+  lqi: \d+")
    child.expect("flags: 0x0")
    child.expect("src_l2addr: 3e:e6:b5:22:fd:0b")
    child.expect("dst_l2addr: 3e:e6:b5:22:fd:0a")
    child.expect("~~ PKT    -  2 snips, total size:  \d+ byte")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, traceback=True))
