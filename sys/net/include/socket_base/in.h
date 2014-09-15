/*
 * Copyright (C) 2014  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * socket_base/in.h - Constants defined by the internet system, per RFC 790,
 *                September 1981, and numerous additions, inspired by
 *                netinet/in.h definitions.
 * @{
 * @file
 * @brief   IP next header constants
 * @see <a href="http://tools.ietf.org/html/rfc790">
 *          RFC790 - Assigned Numbers
 *      </a>
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/netinet_in.h.html">
 *          IEEE Std 1003.1, 2013 Edition - netinet/in.h
 *      </a>
 * @see <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *          IANA - Protocol Numbers
 *      </a>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef SOCKET_BASE_IN_H
#define SOCKET_BASE_IN_H

/*
 * Protocols (RFC 1700) TODO: may be deleted due to some double definition
 *                            in sys/net/sixlowpan/include/sixlowpan/ip.h
 */
#define IPPROTO_IP              (0)               ///<  dummy for IP
#define IPPROTO_HOPOPTS         (0)               ///<  IP6 hop-by-hop options
#define IPPROTO_ICMP            (1)               ///<  control message protocol
#define IPPROTO_IGMP            (2)               ///<  group mgmt protocol
#define IPPROTO_GGP             (3)               ///<  gateway^2 (deprecated)
#define IPPROTO_IPV4            (4)               ///<  IPv4 encapsulation
#define IPPROTO_IPIP            IPPROTO_IPV4      ///<  for compatibility
#define IPPROTO_TCP             (6)               ///<  tcp
#define IPPROTO_ST              (7)               ///<  Stream protocol II
#define IPPROTO_EGP             (8)               ///<  exterior gateway protocol
#define IPPROTO_PIGP            (9)               ///<  private interior gateway
#define IPPROTO_RCCMON          (10)              ///<  BBN RCC Monitoring
#define IPPROTO_NVPII           (11)              ///<  network voice protocol
#define IPPROTO_PUP             (12)              ///<  pup
#define IPPROTO_ARGUS           (13)              ///<  Argus
#define IPPROTO_EMCON           (14)              ///<  EMCON
#define IPPROTO_XNET            (15)              ///<  Cross Net Debugger
#define IPPROTO_CHAOS           (16)              ///<  Chaos
#define IPPROTO_UDP             (17)              ///<  user datagram protocol
#define IPPROTO_MUX             (18)              ///<  Multiplexing
#define IPPROTO_MEAS            (19)              ///<  DCN Measurement Subsystems
#define IPPROTO_HMP             (20)              ///<  Host Monitoring
#define IPPROTO_PRM             (21)              ///<  Packet Radio Measurement
#define IPPROTO_IDP             (22)              ///<  xns idp
#define IPPROTO_TRUNK1          (23)              ///<  Trunk-1
#define IPPROTO_TRUNK2          (24)              ///<  Trunk-2
#define IPPROTO_LEAF1           (25)              ///<  Leaf-1
#define IPPROTO_LEAF2           (26)              ///<  Leaf-2
#define IPPROTO_RDP             (27)              ///<  Reliable Data
#define IPPROTO_IRTP            (28)              ///<  Reliable Transaction
#define IPPROTO_TP              (29)              ///<  tp-4 w/ class negotiation
#define IPPROTO_BLT             (30)              ///<  Bulk Data Transfer
#define IPPROTO_NSP             (31)              ///<  Network Services
#define IPPROTO_INP             (32)              ///<  Merit Internodal
#define IPPROTO_SEP             (33)              ///<  Sequential Exchange
#define IPPROTO_3PC             (34)              ///<  Third Party Connect
#define IPPROTO_IDPR            (35)              ///<  InterDomain Policy Routing
#define IPPROTO_XTP             (36)              ///<  XTP
#define IPPROTO_DDP             (37)              ///<  Datagram Delivery
#define IPPROTO_CMTP            (38)              ///<  Control Message Transport
#define IPPROTO_TPXX            (39)              ///<  TP++ Transport
#define IPPROTO_IL              (40)              ///<  IL transport protocol
#define IPPROTO_IPV6            (41)              ///<  IP6 header
#define IPPROTO_SDRP            (42)              ///<  Source Demand Routing
#define IPPROTO_ROUTING         (43)              ///<  IP6 routing header
#define IPPROTO_FRAGMENT        (44)              ///<  IP6 fragmentation header
#define IPPROTO_IDRP            (45)              ///<  InterDomain Routing
#define IPPROTO_RSVP            (46)              ///<  resource reservation
#define IPPROTO_GRE             (47)              ///<  General Routing Encap.
#define IPPROTO_MHRP            (48)              ///<  Mobile Host Routing
#define IPPROTO_BHA             (49)              ///<  BHA
#define IPPROTO_ESP             (50)              ///<  IP6 Encap Sec. Payload
#define IPPROTO_AH              (51)              ///<  IP6 Auth Header
#define IPPROTO_INLSP           (52)              ///<  Integ. Net Layer Security
#define IPPROTO_SWIPE           (53)              ///<  IP with encryption
#define IPPROTO_NHRP            (54)              ///<  Next Hop Resolution
///<  55-57: Unassigned
#define IPPROTO_ICMPV6          (58)              ///<  ICMP6
#define IPPROTO_NONE            (59)              ///<  IP6 no next header
#define IPPROTO_DSTOPTS         (60)              ///<  IP6 destination option
#define IPPROTO_AHIP            (61)              ///<  any host internal protocol
#define IPPROTO_CFTP            (62)              ///<  CFTP
#define IPPROTO_HELLO           (63)              ///<  "hello" routing protocol
#define IPPROTO_SATEXPAK        (64)              ///<  SATNET/Backroom EXPAK
#define IPPROTO_KRYPTOLAN       (65)              ///<  Kryptolan
#define IPPROTO_RVD             (66)              ///<  Remote Virtual Disk
#define IPPROTO_IPPC            (67)              ///<  Pluribus Packet Core
#define IPPROTO_ADFS            (68)              ///<  Any distributed FS
#define IPPROTO_SATMON          (69)              ///<  Satnet Monitoring
#define IPPROTO_VISA            (70)              ///<  VISA Protocol
#define IPPROTO_IPCV            (71)              ///<  Packet Core Utility
#define IPPROTO_CPNX            (72)              ///<  Comp. Prot. Net. Executive
#define IPPROTO_CPHB            (73)              ///<  Comp. Prot. HeartBeat
#define IPPROTO_WSN             (74)              ///<  Wang Span Network
#define IPPROTO_PVP             (75)              ///<  Packet Video Protocol
#define IPPROTO_BRSATMON        (76)              ///<  BackRoom SATNET Monitoring
#define IPPROTO_ND              (77)              ///<  Sun net disk proto (temp.)
#define IPPROTO_WBMON           (78)              ///<  WIDEBAND Monitoring
#define IPPROTO_WBEXPAK         (79)              ///<  WIDEBAND EXPAK
#define IPPROTO_EON             (80)              ///<  ISO cnlp
#define IPPROTO_VMTP            (81)              ///<  VMTP
#define IPPROTO_SVMTP           (82)              ///<  Secure VMTP
#define IPPROTO_VINES           (83)              ///<  Banyon VINES
#define IPPROTO_TTP             (84)              ///<  TTP
#define IPPROTO_IGP             (85)              ///<  NSFNET-IGP
#define IPPROTO_DGP             (86)              ///<  dissimilar gateway prot.
#define IPPROTO_TCF             (87)              ///<  TCF
#define IPPROTO_IGRP            (88)              ///<  Cisco/GXS IGRP
#define IPPROTO_OSPFIGP         (89)              ///<  OSPFIGP
#define IPPROTO_SRPC            (90)              ///<  Strite RPC protocol
#define IPPROTO_LARP            (91)              ///<  Locus Address Resoloution
#define IPPROTO_MTP             (92)              ///<  Multicast Transport
#define IPPROTO_AX25            (93)              ///<  AX.25 Frames
#define IPPROTO_IPEIP           (94)              ///<  IP encapsulated in IP
#define IPPROTO_MICP            (95)              ///<  Mobile Int.ing control
#define IPPROTO_SCCSP           (96)              ///<  Semaphore Comm. security
#define IPPROTO_ETHERIP         (97)              ///<  Ethernet IP encapsulation
#define IPPROTO_ENCAP           (98)              ///<  encapsulation header
#define IPPROTO_APES            (99)              ///<  any private encr. scheme
#define IPPROTO_GMTP            (100)             ///<  GMTP
#define IPPROTO_IPCOMP          (108)             ///<  payload compression (IPComp)
/**
 *  101-254: Partly Unassigned
 */
#define IPPROTO_PIM             (103)             ///<  Protocol Independent Mcast
#define IPPROTO_PGM             (113)             ///<  PGM
/* 255: Reserved */
/* */
#define IPPROTO_DIVERT          (254)             ///< divert pseudo-protocol (BSD Private, local use, namespace incursion)
#define IPPROTO_RAW             (255)             ///<  raw IP packet
#define IPPROTO_MAX             (256)

#define IPPROTO_DONE            (257)             ///< last return value of *_input(), meaning "all job for this pkt is done".

#define IN_LOOPBACKNET          (127)             ///< official!

#endif /* SOCKET_BASE_IN_H */
