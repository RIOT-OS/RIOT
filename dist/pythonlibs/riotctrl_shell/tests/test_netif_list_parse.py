# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""Tests various example outputs of Ifconfig::ifconfig_list()"""

import riotctrl_shell.netif


def test_ifconfig_list_parser1():
    cmd_output = """
Iface  5  HWaddr: E2:BC:7D:CB:F5:50
          L2-PDU:1500  MTU:1500  HL:64  RTR
          RTR_ADV
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::e0bc:7dff:fecb:f550  scope: link  VAL
          inet6 addr: fe80::e0bc:7dff:fecb:f551  scope: link [anycast]  TNT[2]
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ffcb:f550

          White-listed link layer addresses:
             0: ab:cd:ef:01:23:45

          Statistics for Layer 2
            RX packets 14  bytes 1300
            TX packets 3 (Multicast: 1)  bytes 234
            TX succeeded 3 errors 0
          Statistics for IPv6
            RX packets 14  bytes 1104
            TX packets 3 (Multicast: 1)  bytes 192
            TX succeeded 3 errors 0"""
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 1
    assert "5" in res
    assert len(res["5"]["flags"]) == 2
    assert "RTR" in res["5"]["flags"]
    assert "RTR_ADV" in res["5"]["flags"]
    assert res["5"]["hwaddr"] == "E2:BC:7D:CB:F5:50"
    assert res["5"]["l2_pdu"] == 1500
    assert res["5"]["mtu"] == 1500
    assert res["5"]["hl"] == 64
    assert res["5"]["source_address_length"] == 6
    assert res["5"]["link_type"] == "wired"
    assert len(res["5"]["ipv6_addrs"]) == 2
    assert {"addr": "fe80::e0bc:7dff:fecb:f550", "scope": "link",
            "state": "VAL"} in res["5"]["ipv6_addrs"]
    assert {"addr": "fe80::e0bc:7dff:fecb:f551", "scope": "link",
            "state": "TNT[2]", "anycast": True} in res["5"]["ipv6_addrs"]
    assert len(res["5"]["ipv6_groups"]) == 3
    assert {"addr": "ff02::1"} in res["5"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["5"]["ipv6_groups"]
    assert {"addr": "ff02::1:ffcb:f550"} in res["5"]["ipv6_groups"]
    assert len(res["5"]["whitelist"]) == 1
    # white and blacklist are mutually exclusive
    assert "blacklist" not in res["5"]
    assert "ab:cd:ef:01:23:45" in res["5"]["whitelist"]
    assert len(res["5"]["stats"]) == 2
    assert res["5"]["stats"]["Layer 2"]["rx"]["packets"] == 14
    assert res["5"]["stats"]["Layer 2"]["rx"]["bytes"] == 1300
    assert res["5"]["stats"]["Layer 2"]["tx"]["packets"] == 3
    assert res["5"]["stats"]["Layer 2"]["tx"]["multicast"] == 1
    assert res["5"]["stats"]["Layer 2"]["tx"]["bytes"] == 234
    assert res["5"]["stats"]["Layer 2"]["tx"]["succeeded"] == 3
    assert res["5"]["stats"]["Layer 2"]["tx"]["errors"] == 0
    assert res["5"]["stats"]["IPv6"]["rx"]["packets"] == 14
    assert res["5"]["stats"]["IPv6"]["rx"]["bytes"] == 1104
    assert res["5"]["stats"]["IPv6"]["tx"]["packets"] == 3
    assert res["5"]["stats"]["IPv6"]["tx"]["multicast"] == 1
    assert res["5"]["stats"]["IPv6"]["tx"]["bytes"] == 192
    assert res["5"]["stats"]["IPv6"]["tx"]["succeeded"] == 3
    assert res["5"]["stats"]["IPv6"]["tx"]["errors"] == 0


def test_ifconfig_list_parser2():
    cmd_output = """
Iface  7  HWaddr: 76:F5:98:9F:40:22
          L2-PDU:1500  MTU:1500  HL:64  RTR
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::74f5:98ff:fe9f:4022  scope: link  VAL
          inet6 addr: fe80::2  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff9f:4022
          inet6 group: ff02::1:ff00:2

Iface  6  HWaddr: 2D:4A  Channel: 26  Page: 0  NID: 0x23  PHY: O-QPSK

          Long HWaddr: 5A:9D:93:86:22:08:65:7B
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
          AUTOACK  ACK_REQ  CSMA  L2-PDU:102  MTU:1280  HL:64  RTR
          RTR_ADV  6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::589d:9386:2208:657b  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff08:657b"""
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 2
    assert "7" in res
    assert len(res["7"]["flags"]) == 1
    assert "RTR" in res["7"]["flags"]
    assert res["7"]["source_address_length"] == 6
    assert res["7"]["link_type"] == "wired"
    assert len(res["7"]["ipv6_addrs"]) == 2
    assert {"addr": "fe80::74f5:98ff:fe9f:4022", "scope": "link",
            "state": "VAL"} in res["7"]["ipv6_addrs"]
    assert {"addr": "fe80::2", "scope": "link",
            "state": "VAL"} in res["7"]["ipv6_addrs"]
    assert len(res["7"]["ipv6_groups"]) == 4
    assert {"addr": "ff02::1"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff9f:4022"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff00:2"} in res["7"]["ipv6_groups"]
    assert "6" in res
    assert len(res["6"]["flags"]) == 7
    assert "AUTOACK" in res["6"]["flags"]
    assert "ACK_REQ" in res["6"]["flags"]
    assert "CSMA" in res["6"]["flags"]
    assert "RTR" in res["6"]["flags"]
    assert "RTR_ADV" in res["6"]["flags"]
    assert "6LO" in res["6"]["flags"]
    assert "IPHC" in res["6"]["flags"]
    assert res["6"]["hwaddr"] == "2D:4A"
    assert res["6"]["channel"] == 26
    assert res["6"]["page"] == 0
    assert res["6"]["nid"] == 0x23
    assert res["6"]["phy"] == "O-QPSK"
    assert res["6"]["long_hwaddr"] == "5A:9D:93:86:22:08:65:7B"
    assert res["6"]["tx_power"] == "0dBm"
    assert res["6"]["state"] == "IDLE"
    assert res["6"]["max_retrans"] == 3
    assert res["6"]["csma_retries"] == 4
    assert res["6"]["l2_pdu"] == 102
    assert res["6"]["mtu"] == 1280
    assert res["6"]["hl"] == 64
    assert res["6"]["source_address_length"] == 8
    assert res["6"]["link_type"] == "wireless"
    assert len(res["6"]["ipv6_addrs"]) == 1
    assert {"addr": "fe80::589d:9386:2208:657b", "scope": "link",
            "state": "VAL"} in res["6"]["ipv6_addrs"]
    assert {"addr": "ff02::1"} in res["6"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["6"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff08:657b"} in res["6"]["ipv6_groups"]


def test_ifconfig_list_parser3():
    cmd_output = """
 ifconfig
Iface  3  HWaddr: 26:01:24:C0  Frequency: 869524963Hz  BW: 125kHz  SF: 12  CR: 4/5  Link: up
           TX-Power: 14dBm  State: SLEEP  Demod margin.: 0  Num gateways.: 0
          IQ_INVERT
          RX_SINGLE  OTAA  L2-PDU:2559
          """     # noqa: E501
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 1
    assert "3" in res
    print(res)
    assert len(res["3"]["flags"]) == 3
    assert "IQ_INVERT" in res["3"]["flags"]
    assert "RX_SINGLE" in res["3"]["flags"]
    assert "OTAA" in res["3"]["flags"]
    assert res["3"]["hwaddr"] == "26:01:24:C0"
    assert res["3"]["frequency"] == "869524963Hz"
    assert res["3"]["bw"] == "125kHz"
    assert res["3"]["sf"] == 12
    assert res["3"]["cr"] == "4/5"
    assert res["3"]["link"] == "up"
    assert res["3"]["tx_power"] == "14dBm"
    assert res["3"]["state"] == "SLEEP"
    assert res["3"]["demod_margin"] == 0
    assert res["3"]["num_gateways"] == 0
    assert res["3"]["l2_pdu"] == 2559


def test_ifconfig_list_parser4():
    cmd_output = """
> ifconfig 7
2020-07-06 12:13:37,636 #  ifconfig 7
2020-07-06 12:13:37,653 # Iface  7  HWaddr: 3A:A4  Channel: 26  NID: 0x23  PHY: MR-O-QPSK
2020-07-06 12:13:37,654 #            chip rate: 2000  rate mode: 0
2020-07-06 12:13:37,655 #           Long HWaddr: 22:68:31:23:59:F5:D2:38
2020-07-06 12:13:37,669 #            TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
2020-07-06 12:13:37,671 #           AUTOACK  ACK_REQ  CSMA  L2-PDU:2022  MTU:1280  HL:64  RTR
2020-07-06 12:13:37,671 #           6LO  IPHC
2020-07-06 12:13:37,672 #           Source address length: 8
2020-07-06 12:13:37,684 #           Link type: wireless
2020-07-06 12:13:37,686 #           inet6 addr: fe80::2068:3123:59f5:d238  scope: link  VAL
2020-07-06 12:13:37,686 #           inet6 group: ff02::2
2020-07-06 12:13:37,687 #           inet6 group: ff02::1
2020-07-06 12:13:37,688 #           inet6 group: ff02::1:fff5:d238
2020-07-06 12:13:37,700 #
2020-07-06 12:13:37,701 #           Statistics for Layer 2
2020-07-06 12:13:37,702 #             RX packets 1  bytes 43
2020-07-06 12:13:37,703 #             TX packets 6 (Multicast: 6)  bytes 258
2020-07-06 12:13:37,704 #             TX succeeded 6 errors 0
2020-07-06 12:13:37,716 #           Statistics for IPv6
2020-07-06 12:13:37,717 #             RX packets 1  bytes 64
2020-07-06 12:13:37,718 #             TX packets 6 (Multicast: 6)  bytes 384
2020-07-06 12:13:37,719 #             TX succeeded 6 errors 0 """   # noqa: E501
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 1
    assert "7" in res
    assert len(res["7"]["flags"]) == 6
    assert "AUTOACK" in res["7"]["flags"]
    assert "ACK_REQ" in res["7"]["flags"]
    assert "CSMA" in res["7"]["flags"]
    assert "RTR" in res["7"]["flags"]
    assert "6LO" in res["7"]["flags"]
    assert "IPHC" in res["7"]["flags"]
    assert res["7"]["hwaddr"] == "3A:A4"
    assert res["7"]["channel"] == 26
    assert "page" not in res["7"]
    assert res["7"]["nid"] == 0x23
    assert res["7"]["phy"] == "MR-O-QPSK"
    assert res["7"]["chip_rate"] == 2000
    assert res["7"]["rate_mode"] == 0
    assert res["7"]["long_hwaddr"] == "22:68:31:23:59:F5:D2:38"
    assert res["7"]["tx_power"] == "0dBm"
    assert res["7"]["state"] == "IDLE"
    assert res["7"]["max_retrans"] == 3
    assert res["7"]["csma_retries"] == 4
    assert res["7"]["l2_pdu"] == 2022
    assert res["7"]["mtu"] == 1280
    assert res["7"]["hl"] == 64
    assert res["7"]["source_address_length"] == 8
    assert res["7"]["link_type"] == "wireless"
    assert len(res["7"]["ipv6_addrs"]) == 1
    assert {"addr": "fe80::2068:3123:59f5:d238", "scope": "link",
            "state": "VAL"} in res["7"]["ipv6_addrs"]
    assert len(res["7"]["ipv6_groups"]) == 3
    assert {"addr": "ff02::1"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::1:fff5:d238"} in res["7"]["ipv6_groups"]
    assert len(res["7"]["stats"]) == 2
    assert res["7"]["stats"]["Layer 2"]["rx"]["packets"] == 1
    assert res["7"]["stats"]["Layer 2"]["rx"]["bytes"] == 43
    assert res["7"]["stats"]["Layer 2"]["tx"]["packets"] == 6
    assert res["7"]["stats"]["Layer 2"]["tx"]["multicast"] == 6
    assert res["7"]["stats"]["Layer 2"]["tx"]["bytes"] == 258
    assert res["7"]["stats"]["Layer 2"]["tx"]["succeeded"] == 6
    assert res["7"]["stats"]["Layer 2"]["tx"]["errors"] == 0
    assert res["7"]["stats"]["IPv6"]["rx"]["packets"] == 1
    assert res["7"]["stats"]["IPv6"]["rx"]["bytes"] == 64
    assert res["7"]["stats"]["IPv6"]["tx"]["packets"] == 6
    assert res["7"]["stats"]["IPv6"]["tx"]["multicast"] == 6
    assert res["7"]["stats"]["IPv6"]["tx"]["bytes"] == 384
    assert res["7"]["stats"]["IPv6"]["tx"]["succeeded"] == 6
    assert res["7"]["stats"]["IPv6"]["tx"]["errors"] == 0


def test_ifconfig_list_parser5():
    cmd_output = """
> ifconfig 7
Iface  7  HWaddr: 3A:A4  Channel: 26  NID: 0x23  PHY: MR-OFDM
           Option: 1  MCS: 0 (BPSK, rate 1/2, 4x frequency repetition)
          Long HWaddr: 22:68:31:23:59:F5:D2:38
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
          AUTOACK  ACK_REQ  CSMA  L2-PDU:2022  MTU:1280  HL:64  RTR
          6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::2068:3123:59f5:d238  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:fff5:d238

          Statistics for Layer 2
            RX packets 1  bytes 43
            TX packets 7 (Multicast: 7)  bytes 301
            TX succeeded 7 errors 0
          Statistics for IPv6
            RX packets 1  bytes 64
            TX packets 7 (Multicast: 7)  bytes 448
            TX succeeded 7 errors 0"""  # noqa: E501
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    # not much changed compared to test_ifconfig_list_parser4, so only check
    # mainly differences
    assert len(res) == 1
    assert "7" in res
    assert len(res["7"]["flags"]) == 6
    assert "page" not in res["7"]
    assert res["7"]["phy"] == "MR-OFDM"
    assert res["7"]["option"] == 1
    assert res["7"]["mcs"] == "0 (BPSK, rate 1/2, 4x frequency repetition)"
    assert "chip_rate" not in res["7"]
    assert "rate_mode" not in res["7"]
    assert len(res["7"]["ipv6_addrs"]) == 1
    assert len(res["7"]["ipv6_groups"]) == 3
    assert len(res["7"]["stats"]) == 2
    assert res["7"]["stats"]["Layer 2"]["rx"]["packets"] == 1
    assert res["7"]["stats"]["Layer 2"]["rx"]["bytes"] == 43
    assert res["7"]["stats"]["Layer 2"]["tx"]["packets"] == 7
    assert res["7"]["stats"]["Layer 2"]["tx"]["multicast"] == 7
    assert res["7"]["stats"]["Layer 2"]["tx"]["bytes"] == 301
    assert res["7"]["stats"]["Layer 2"]["tx"]["succeeded"] == 7
    assert res["7"]["stats"]["Layer 2"]["tx"]["errors"] == 0
    assert res["7"]["stats"]["IPv6"]["rx"]["packets"] == 1
    assert res["7"]["stats"]["IPv6"]["rx"]["bytes"] == 64
    assert res["7"]["stats"]["IPv6"]["tx"]["packets"] == 7
    assert res["7"]["stats"]["IPv6"]["tx"]["multicast"] == 7
    assert res["7"]["stats"]["IPv6"]["tx"]["bytes"] == 448
    assert res["7"]["stats"]["IPv6"]["tx"]["succeeded"] == 7
    assert res["7"]["stats"]["IPv6"]["tx"]["errors"] == 0


def test_ifconfig_list_parser6():
    cmd_output = """
 ifconfig 7
Iface  7  HWaddr: 3A:A4  Channel: 26  NID: 0x23  PHY: MR-FSK
           modulation index: 1  2-FSK  symbol rate: 50 kHz  FEC: none  BW: 400kHz
          Long HWaddr: 22:68:31:23:59:F5:D2:38
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
          AUTOACK  ACK_REQ  CSMA  L2-PDU:2022 MTU:1280  HL:64  RTR
          6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::2068:3123:59f5:d238  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:fff5:d238

          Statistics for Layer 2
            RX packets 1  bytes 43
            TX packets 8 (Multicast: 8)  bytes 344
            TX succeeded 8 errors 0
          Statistics for IPv6
            RX packets 1  bytes 64
            TX packets 8 (Multicast: 8)  bytes 512
            TX succeeded 8 errors 0"""  # noqa: E501
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    # not much changed compared to test_ifconfig_list_parser4 and 5, so only
    # check mainly differences
    assert len(res) == 1
    assert "7" in res
    assert len(res["7"]["flags"]) == 7
    assert "AUTOACK" in res["7"]["flags"]
    assert "ACK_REQ" in res["7"]["flags"]
    assert "CSMA" in res["7"]["flags"]
    assert "RTR" in res["7"]["flags"]
    assert "6LO" in res["7"]["flags"]
    assert "IPHC" in res["7"]["flags"]
    assert "2-FSK" in res["7"]["flags"]
    assert "page" not in res["7"]
    assert res["7"]["phy"] == "MR-FSK"
    assert res["7"]["modulation_index"] == 1
    assert res["7"]["symbol_rate"] == "50 kHz"
    assert res["7"]["fec"] == "none"
    assert res["7"]["bw"] == "400kHz"
    assert "chip_rate" not in res["7"]
    assert "rate_mode" not in res["7"]
    assert "option" not in res["7"]
    assert "mcs" not in res["7"]
    assert len(res["7"]["ipv6_addrs"]) == 1
    assert len(res["7"]["ipv6_groups"]) == 3


def test_ifconfig_list_parser7():
    cmd_output = """
Iface  8  HWaddr: 45:5B  Channel: 26  NID: 0x23
          Long HWaddr: 00:5A:45:50:0A:00:45:5B
          L2-PDU:102  MTU:1280  HL:64  RTR
          6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::25a:4550:a00:455b  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff00:455b

          Statistics for Layer 2
            RX packets 0  bytes 0
            TX packets 2 (Multicast: 2)  bytes 43
            TX succeeded 3 errors 0
          Statistics for IPv6
            RX packets 0  bytes 0
            TX packets 2 (Multicast: 2)  bytes 128
            TX succeeded 2 errors 0

Iface  7  HWaddr: B6:4F:A1:0E:8F:CC
          L2-PDU:1500  MTU:1500  HL:64  RTR
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::b44f:a1ff:fe0e:8fcc  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff0e:8fcc

          Statistics for Layer 2
            RX packets 4  bytes 480
            TX packets 3 (Multicast: 3)  bytes 210
            TX succeeded 3 errors 0
          Statistics for IPv6
            RX packets 4  bytes 424
            TX packets 3 (Multicast: 3)  bytes 168
            TX succeeded 3 errors 0"""
    parser = riotctrl_shell.netif.IfconfigListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 2
    assert "7" in res
    assert "8" in res
