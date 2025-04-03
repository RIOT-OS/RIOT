/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_PROTNUM_H
#define NET_PROTNUM_H
/**
 * @defgroup    net_protnum  Protocol Numbers
 * @ingroup     net
 * @brief       Defines for the Protocol Numbers as they are used in the
 *              IPv4 protocol field and the IPv6 next header field
 *              (ipv6_hdr_t::nh).
 * @see         <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *                  IANA, Assigned Internet Protocol Numbers
 *              </a>
 * @note        Last Updated: 2015-01-06
 * @{
 *
 * @file
 * @brief   Protocol number definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PROTNUM_IPV6_EXT_HOPOPT      (0)        /**< IPv6 Hop-by-Hop Option */
#define PROTNUM_ICMP                 (1)        /**< Internet Control Message */
#define PROTNUM_IGMP                 (2)        /**< Internet Group Management */
#define PROTNUM_GGP                  (3)        /**< Gateway-to-Gateway */
#define PROTNUM_IPV4                 (4)        /**< IPv4 encapsulation */
#define PROTNUM_ST                   (5)        /**< Stream */
#define PROTNUM_TCP                  (6)        /**< Transmission Control */
#define PROTNUM_CBT                  (7)        /**< CBT */
#define PROTNUM_EGP                  (8)        /**< Exterior Gateway Protocol */
#define PROTNUM_IGP                  (9)        /**< any private interior gateway
                                                 *   (Cisco IGRP) */
#define PROTNUM_BBN_RCC_MON          (10)       /**< BBN RCC Monitoring */
#define PROTNUM_NVP_II               (11)       /**< Network Voice Protocol */
#define PROTNUM_PUP                  (12)       /**< PUP */
#define PROTNUM_ARGUS                (13)       /**< ARGUS */
#define PROTNUM_EMCON                (14)       /**< EMCON */
#define PROTNUM_XNET                 (15)       /**< Cross Net Debugger */
#define PROTNUM_CHAOS                (16)       /**< Chaos */
#define PROTNUM_UDP                  (17)       /**< User Datagram */
#define PROTNUM_MUX                  (18)       /**< Multiplexing */
#define PROTNUM_DCN_MEAS             (19)       /**< DCN Measurement Subsystems */
#define PROTNUM_HMP                  (20)       /**< Host Monitoring */
#define PROTNUM_PRM                  (21)       /**< Packet Radio Measurement */
#define PROTNUM_XNS_IDP              (22)       /**< XEROX NS IDP */
#define PROTNUM_TRUNK_1              (23)       /**< Trunk-1 */
#define PROTNUM_TRUNK_2              (24)       /**< Trunk-2 */
#define PROTNUM_LEAF_1               (25)       /**< Leaf-1 */
#define PROTNUM_LEAF_2               (26)       /**< Leaf-2 */
#define PROTNUM_RDP                  (27)       /**< Reliable Data Protocol */
#define PROTNUM_IRTP                 (28)       /**< Internet Reliable Transaction */
#define PROTNUM_ISO_TP4              (29)       /**< ISO Transport Protocol Class 4 */
#define PROTNUM_NETBLT               (30)       /**< Bulk Data Transfer Protocol */
#define PROTNUM_MFE_NSP              (31)       /**< MFE Network Services Protocol */
#define PROTNUM_MERIT_INP            (32)       /**< MERIT Internodal Protocol */
#define PROTNUM_DCCP                 (33)       /**< Datagram Congestion Control Protocol */
#define PROTNUM_3PC                  (34)       /**< Third Party Connect Protocol */
#define PROTNUM_IDPR                 (35)       /**< Inter-Domain Policy Routing Protocol */
#define PROTNUM_XTP                  (36)       /**< XTP */
#define PROTNUM_DDP                  (37)       /**< Datagram Delivery Protocol */
#define PROTNUM_IDPR_CMTP            (38)       /**< IDPR Control Message Transport Proto */
#define PROTNUM_TPPLUSPLUS           (39)       /**< TP++ Transport Protocol */
#define PROTNUM_IL                   (40)       /**< IL Transport Protocol */
#define PROTNUM_IPV6                 (41)       /**< IPv6 encapsulation */
#define PROTNUM_SDRP                 (42)       /**< Source Demand Routing Protocol */
#define PROTNUM_IPV6_EXT_RH          (43)       /**< Routing Header for IPv6 */
#define PROTNUM_IPV6_EXT_FRAG        (44)       /**< Fragment Header for IPv6 */
#define PROTNUM_IDRP                 (45)       /**< Inter-Domain Routing Protocol */
#define PROTNUM_RSVP                 (46)       /**< Reservation Protocol */
#define PROTNUM_GRE                  (47)       /**< Generic Routing Encapsulation */
#define PROTNUM_DSR                  (48)       /**< Dynamic Source Routing Protocol */
#define PROTNUM_BNA                  (49)       /**< BNA */
#define PROTNUM_IPV6_EXT_ESP         (50)       /**< IPv6 Encap Security Payload
                                                 *   Extension Header */
#define PROTNUM_IPV6_EXT_AH          (51)       /**< IPv6 Authentication Extension
                                                 *   Header */
#define PROTNUM_I_NLSP               (52)       /**< Integrated Net Layer Security
                                                 *   TUBA */
#define PROTNUM_SWIPE                (53)       /**< IP with Encryption (deprecated) */
#define PROTNUM_NARP                 (54)       /**< NBMA Address Resolution Protocol */
#define PROTNUM_MOBILE               (55)       /**< IP Mobility */
#define PROTNUM_TLSP                 (56)       /**< Transport Layer Security Protocol */
#define PROTNUM_SKIP                 (57)       /**< SKIP */
#define PROTNUM_ICMPV6               (58)       /**< ICMP for IPv6 */
#define PROTNUM_IPV6_NONXT           (59)       /**< No Next Header for IPv6 */
#define PROTNUM_IPV6_EXT_DST         (60)       /**< IPv6 Extension Header:
                                                 *   Destination Options */
#define PROTNUM_CFTP                 (62)       /**< CFTP */
#define PROTNUM_SAT_EXPAK            (64)       /**< SATNET and Backroom EXPAK */
#define PROTNUM_KRYPTOLAN            (65)       /**< Kryptolan */
#define PROTNUM_RVD                  (66)       /**< MIT Remote Virtual Disk Protocol */
#define PROTNUM_IPPC                 (67)       /**< Internet Pluribus Packet Core */
#define PROTNUM_SAT_MON              (69)       /**< SATNET Monitoring */
#define PROTNUM_VISA                 (70)       /**< VISA Protocol */
#define PROTNUM_IPCV                 (71)       /**< Internet Packet Core Utility */
#define PROTNUM_CPNX                 (72)       /**< Computer Protocol Network Executive */
#define PROTNUM_CPHB                 (73)       /**< Computer Protocol Heart Beat */
#define PROTNUM_WSN                  (74)       /**< Wang Span Network */
#define PROTNUM_PVP                  (75)       /**< Packet Video Protocol */
#define PROTNUM_BR_SAT_MON           (76)       /**< Backroom SATNET Monitoring */
#define PROTNUM_SUN_ND               (77)       /**< SUN ND PROTOCOL-Temporary */
#define PROTNUM_WB_MON               (78)       /**< WIDEBAND Monitoring */
#define PROTNUM_WB_EXPAK             (79)       /**< WIDEBAND EXPAK */
#define PROTNUM_ISO_IP               (80)       /**< ISO Internet Protocol */
#define PROTNUM_VMTP                 (81)       /**< VMTP */
#define PROTNUM_SECURE_VMTP          (82)       /**< SECURE-VMTP */
#define PROTNUM_VINES                (83)       /**< VINES */
#define PROTNUM_TTP                  (84)       /**< Transaction Transport Protocol */
#define PROTNUM_IPTM                 (84)       /**< Internet Protocol Traffic Manager */
#define PROTNUM_NSFNET_IGP           (85)       /**< NSFNET-IGP */
#define PROTNUM_DGP                  (86)       /**< Dissimilar Gateway Protocol */
#define PROTNUM_TCF                  (87)       /**< TCF */
#define PROTNUM_EIGRP                (88)       /**< EIGRP */
#define PROTNUM_OSPFIGP              (89)       /**< OSPFIGP */
#define PROTNUM_SPRITE_RPC           (90)       /**< Sprite RPC Protocol */
#define PROTNUM_LARP                 (91)       /**< Locus Address Resolution Protocol */
#define PROTNUM_MTP                  (92)       /**< Multicast Transport Protocol */
#define PROTNUM_AX_25                (93)       /**< AX.25 Frames */
#define PROTNUM_IPIP                 (94)       /**< IP-within-IP Encapsulation Protocol */
#define PROTNUM_MICP                 (95)       /**< Mobile Internetworking Control Pro
                                                 *   (deprecated) */
#define PROTNUM_SCC_SP               (96)       /**< Semaphore Communications Sec Pro. */
#define PROTNUM_ETHERIP              (97)       /**< Ethernet-within-IP Encapsulation */
#define PROTNUM_ENCAP                (98)       /**< Encapsulation Header */
#define PROTNUM_GMTP                 (100)      /**< GMTP */
#define PROTNUM_IFMP                 (101)      /**< Ipsilon Flow Management Protocol */
#define PROTNUM_PNNI                 (102)      /**< PNNI over IP */
#define PROTNUM_PIM                  (103)      /**< Protocol Independent Multicast */
#define PROTNUM_ARIS                 (104)      /**< ARIS */
#define PROTNUM_SCPS                 (105)      /**< SCPS */
#define PROTNUM_QNX                  (106)      /**< QNX */
#define PROTNUM_A_N                  (107)      /**< Active Networks */
#define PROTNUM_IPCOMP               (108)      /**< IP Payload Compression Protocol */
#define PROTNUM_SNP                  (109)      /**< Sitara Networks Protocol */
#define PROTNUM_COMPAQ_PEER          (110)      /**< Compaq Peer Protocol */
#define PROTNUM_IPX_IN_IP            (111)      /**< IPX in IP */
#define PROTNUM_VRRP                 (112)      /**< Virtual Router Redundancy Protocol */
#define PROTNUM_PGM                  (113)      /**< PGM Reliable Transport Protocol */
#define PROTNUM_L2TP                 (115)      /**< Layer Two Tunneling Protocol */
#define PROTNUM_DDX                  (116)      /**< D-II Data Exchange (DDX) */
#define PROTNUM_IATP                 (117)      /**< Interactive Agent Transfer Protocol */
#define PROTNUM_STP                  (118)      /**< Schedule Transfer Protocol */
#define PROTNUM_SRP                  (119)      /**< SpectraLink Radio Protocol */
#define PROTNUM_UTI                  (120)      /**< UTI */
#define PROTNUM_SMP                  (121)      /**< Simple Message Protocol */
#define PROTNUM_SM                   (122)      /**< Simple Multicast Protocol */
#define PROTNUM_PTP                  (123)      /**< Performance Transparency Protocol */
#define PROTNUM_ISIS_OVER_IPV4       (124)      /**< ISIS over IPv4 */
#define PROTNUM_FIRE                 (125)      /**< FIRE */
#define PROTNUM_CRTP                 (126)      /**< Combat Radio Transport Protocol */
#define PROTNUM_CRUDP                (127)      /**< Combat Radio User Datagram */
#define PROTNUM_SSCOPMCE             (128)      /**< SSCOPMCE */
#define PROTNUM_IPLT                 (129)      /**< IPLT */
#define PROTNUM_SPS                  (130)      /**< Secure Packet Shield */
#define PROTNUM_PIPE                 (131)      /**< Private IP Encapsulation within IP */
#define PROTNUM_SCTP                 (132)      /**< Stream Control Transmission Protocol */
#define PROTNUM_FC                   (133)      /**< Fibre Channel */
#define PROTNUM_RSVP_E2E_IGNORE      (134)      /**< RSVP-E2E-IGNORE */
#define PROTNUM_IPV6_EXT_MOB         (135)      /**< IPv6 Mobility Extension Header */
#define PROTNUM_UDPLITE              (136)      /**< UDPLite */
#define PROTNUM_MPLS_IN_IP           (137)      /**< MPLS-in-IP */
#define PROTNUM_MANET                (138)      /**< MANET Protocols */
#define PROTNUM_HIP                  (139)      /**< Host Identity Protocol */
#define PROTNUM_SHIM6                (140)      /**< Shim6 Protocol */
#define PROTNUM_WESP                 (141)      /**< Wrapped Encapsulating Security Payload */
#define PROTNUM_ROHC                 (142)      /**< Robust Header Compression */
#define PROTNUM_RESERVED             (255)      /**< Reserved */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_PROTNUM_H */
