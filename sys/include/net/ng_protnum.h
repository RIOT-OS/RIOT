/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_protnum  Protocol Numbers
 * @ingroup     net
 * @brief       Defines for the Protocol Numbers as they are used in the
 *              IPv4 protocol field and the IPv6 next header field
 *              (ng_ipv6_hdr_t::nh).
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
#ifndef NG_PROTNUM_H_
#define NG_PROTNUM_H_


#ifdef __cplusplus
extern "C" {
#endif

#define NG_PROTNUM_IPV6_EXT_HOPOPT      (0)     /**< IPv6 Hop-by-Hop Option */
#define NG_PROTNUM_ICMP                 (1)     /**< Internet Control Message */
#define NG_PROTNUM_IGMP                 (2)     /**< Internet Group Management */
#define NG_PROTNUM_GGP                  (3)     /**< Gateway-to-Gateway */
#define NG_PROTNUM_IPV4                 (4)     /**< IPv4 encapsulation */
#define NG_PROTNUM_ST                   (5)     /**< Stream */
#define NG_PROTNUM_TCP                  (6)     /**< Transmission Control */
#define NG_PROTNUM_CBT                  (7)     /**< CBT */
#define NG_PROTNUM_EGP                  (8)     /**< Exterior Gateway Protocol */
#define NG_PROTNUM_IGP                  (9)     /**< any private interior gateway
                                                 *   (Cisco IGRP) */
#define NG_PROTNUM_BBN_RCC_MON          (10)    /**< BBN RCC Monitoring */
#define NG_PROTNUM_NVP_II               (11)    /**< Network Voice Protocol */
#define NG_PROTNUM_PUP                  (12)    /**< PUP */
#define NG_PROTNUM_ARGUS                (13)    /**< ARGUS */
#define NG_PROTNUM_EMCON                (14)    /**< EMCON */
#define NG_PROTNUM_XNET                 (15)    /**< Cross Net Debugger */
#define NG_PROTNUM_CHAOS                (16)    /**< Chaos */
#define NG_PROTNUM_UDP                  (17)    /**< User Datagram */
#define NG_PROTNUM_MUX                  (18)    /**< Multiplexing */
#define NG_PROTNUM_DCN_MEAS             (19)    /**< DCN Measurement Subsystems */
#define NG_PROTNUM_HMP                  (20)    /**< Host Monitoring */
#define NG_PROTNUM_PRM                  (21)    /**< Packet Radio Measurement */
#define NG_PROTNUM_XNS_IDP              (22)    /**< XEROX NS IDP */
#define NG_PROTNUM_TRUNK_1              (23)    /**< Trunk-1 */
#define NG_PROTNUM_TRUNK_2              (24)    /**< Trunk-2 */
#define NG_PROTNUM_LEAF_1               (25)    /**< Leaf-1 */
#define NG_PROTNUM_LEAF_2               (26)    /**< Leaf-2 */
#define NG_PROTNUM_RDP                  (27)    /**< Reliable Data Protocol */
#define NG_PROTNUM_IRTP                 (28)    /**< Internet Reliable Transaction */
#define NG_PROTNUM_ISO_TP4              (29)    /**< ISO Transport Protocol Class 4 */
#define NG_PROTNUM_NETBLT               (30)    /**< Bulk Data Transfer Protocol */
#define NG_PROTNUM_MFE_NSP              (31)    /**< MFE Network Services Protocol */
#define NG_PROTNUM_MERIT_INP            (32)    /**< MERIT Internodal Protocol */
#define NG_PROTNUM_DCCP                 (33)    /**< Datagram Congestion Control Protocol */
#define NG_PROTNUM_3PC                  (34)    /**< Third Party Connect Protocol */
#define NG_PROTNUM_IDPR                 (35)    /**< Inter-Domain Policy Routing Protocol */
#define NG_PROTNUM_XTP                  (36)    /**< XTP */
#define NG_PROTNUM_DDP                  (37)    /**< Datagram Delivery Protocol */
#define NG_PROTNUM_IDPR_CMTP            (38)    /**< IDPR Control Message Transport Proto */
#define NG_PROTNUM_TPPLUSPLUS           (39)    /**< TP++ Transport Protocol */
#define NG_PROTNUM_IL                   (40)    /**< IL Transport Protocol */
#define NG_PROTNUM_IPV6                 (41)    /**< IPv6 encapsulation */
#define NG_PROTNUM_SDRP                 (42)    /**< Source Demand Routing Protocol */
#define NG_PROTNUM_IPV6_EXT_ROUTE       (43)    /**< Routing Header for IPv6 */
#define NG_PROTNUM_IPV6_EXT_FRAG        (44)    /**< Fragment Header for IPv6 */
#define NG_PROTNUM_IDRP                 (45)    /**< Inter-Domain Routing Protocol */
#define NG_PROTNUM_RSVP                 (46)    /**< Reservation Protocol */
#define NG_PROTNUM_GRE                  (47)    /**< Generic Routing Encapsulation */
#define NG_PROTNUM_DSR                  (48)    /**< Dynamic Source Routing Protocol */
#define NG_PROTNUM_BNA                  (49)    /**< BNA */
#define NG_PROTNUM_IPV6_EXT_ESP         (50)    /**< IPv6 Encap Security Payload
                                                 *   Extension Header */
#define NG_PROTNUM_IPV6_EXT_AH          (51)    /**< IPv6 Authentication Extension
                                                 *   Header */
#define NG_PROTNUM_I_NLSP               (52)    /**< Integrated Net Layer Security
                                                 *   TUBA */
#define NG_PROTNUM_SWIPE                (53)    /**< IP with Encryption (deprecated) */
#define NG_PROTNUM_NARP                 (54)    /**< NBMA Address Resolution Protocol */
#define NG_PROTNUM_MOBILE               (55)    /**< IP Mobility */
#define NG_PROTNUM_TLSP                 (56)    /**< Transport Layer Security Protocol */
#define NG_PROTNUM_SKIP                 (57)    /**< SKIP */
#define NG_PROTNUM_ICMPV6               (58)    /**< ICMP for IPv6 */
#define NG_PROTNUM_IPV6_NONXT           (59)    /**< No Next Header for IPv6 */
#define NG_PROTNUM_IPV6_EXT_DEST_OPTS   (60)    /**< IPv6 Extension Header:
                                                 *   Destination Options */
#define NG_PROTNUM_CFTP                 (62)    /**< CFTP */
#define NG_PROTNUM_SAT_EXPAK            (64)    /**< SATNET and Backroom EXPAK */
#define NG_PROTNUM_KRYPTOLAN            (65)    /**< Kryptolan */
#define NG_PROTNUM_RVD                  (66)    /**< MIT Remote Virtual Disk Protocol */
#define NG_PROTNUM_IPPC                 (67)    /**< Internet Pluribus Packet Core */
#define NG_PROTNUM_SAT_MON              (69)    /**< SATNET Monitoring */
#define NG_PROTNUM_VISA                 (70)    /**< VISA Protocol */
#define NG_PROTNUM_IPCV                 (71)    /**< Internet Packet Core Utility */
#define NG_PROTNUM_CPNX                 (72)    /**< Computer Protocol Network Executive */
#define NG_PROTNUM_CPHB                 (73)    /**< Computer Protocol Heart Beat */
#define NG_PROTNUM_WSN                  (74)    /**< Wang Span Network */
#define NG_PROTNUM_PVP                  (75)    /**< Packet Video Protocol */
#define NG_PROTNUM_BR_SAT_MON           (76)    /**< Backroom SATNET Monitoring */
#define NG_PROTNUM_SUN_ND               (77)    /**< SUN ND PROTOCOL-Temporary */
#define NG_PROTNUM_WB_MON               (78)    /**< WIDEBAND Monitoring */
#define NG_PROTNUM_WB_EXPAK             (79)    /**< WIDEBAND EXPAK */
#define NG_PROTNUM_ISO_IP               (80)    /**< ISO Internet Protocol */
#define NG_PROTNUM_VMTP                 (81)    /**< VMTP */
#define NG_PROTNUM_SECURE_VMTP          (82)    /**< SECURE-VMTP */
#define NG_PROTNUM_VINES                (83)    /**< VINES */
#define NG_PROTNUM_TTP                  (84)    /**< Transaction Transport Protocol */
#define NG_PROTNUM_IPTM                 (84)    /**< Internet Protocol Traffic Manager */
#define NG_PROTNUM_NSFNET_IGP           (85)    /**< NSFNET-IGP */
#define NG_PROTNUM_DGP                  (86)    /**< Dissimilar Gateway Protocol */
#define NG_PROTNUM_TCF                  (87)    /**< TCF */
#define NG_PROTNUM_EIGRP                (88)    /**< EIGRP */
#define NG_PROTNUM_OSPFIGP              (89)    /**< OSPFIGP */
#define NG_PROTNUM_SPRITE_RPC           (90)    /**< Sprite RPC Protocol */
#define NG_PROTNUM_LARP                 (91)    /**< Locus Address Resolution Protocol */
#define NG_PROTNUM_MTP                  (92)    /**< Multicast Transport Protocol */
#define NG_PROTNUM_AX_25                (93)    /**< AX.25 Frames */
#define NG_PROTNUM_IPIP                 (94)    /**< IP-within-IP Encapsulation Protocol */
#define NG_PROTNUM_MICP                 (95)    /**< Mobile Internetworking Control Pro
                                                 *   (deprecated) */
#define NG_PROTNUM_SCC_SP               (96)    /**< Semaphore Communications Sec Pro. */
#define NG_PROTNUM_ETHERIP              (97)    /**< Ethernet-within-IP Encapsulation */
#define NG_PROTNUM_ENCAP                (98)    /**< Encapsulation Header */
#define NG_PROTNUM_GMTP                 (100)   /**< GMTP */
#define NG_PROTNUM_IFMP                 (101)   /**< Ipsilon Flow Management Protocol */
#define NG_PROTNUM_PNNI                 (102)   /**< PNNI over IP */
#define NG_PROTNUM_PIM                  (103)   /**< Protocol Independent Multicast */
#define NG_PROTNUM_ARIS                 (104)   /**< ARIS */
#define NG_PROTNUM_SCPS                 (105)   /**< SCPS */
#define NG_PROTNUM_QNX                  (106)   /**< QNX */
#define NG_PROTNUM_A_N                  (107)   /**< Active Networks */
#define NG_PROTNUM_IPCOMP               (108)   /**< IP Payload Compression Protocol */
#define NG_PROTNUM_SNP                  (109)   /**< Sitara Networks Protocol */
#define NG_PROTNUM_COMPAQ_PEER          (110)   /**< Compaq Peer Protocol */
#define NG_PROTNUM_IPX_IN_IP            (111)   /**< IPX in IP */
#define NG_PROTNUM_VRRP                 (112)   /**< Virtual Router Redundancy Protocol */
#define NG_PROTNUM_PGM                  (113)   /**< PGM Reliable Transport Protocol */
#define NG_PROTNUM_L2TP                 (115)   /**< Layer Two Tunneling Protocol */
#define NG_PROTNUM_DDX                  (116)   /**< D-II Data Exchange (DDX) */
#define NG_PROTNUM_IATP                 (117)   /**< Interactive Agent Transfer Protocol */
#define NG_PROTNUM_STP                  (118)   /**< Schedule Transfer Protocol */
#define NG_PROTNUM_SRP                  (119)   /**< SpectraLink Radio Protocol */
#define NG_PROTNUM_UTI                  (120)   /**< UTI */
#define NG_PROTNUM_SMP                  (121)   /**< Simple Message Protocol */
#define NG_PROTNUM_SM                   (122)   /**< Simple Multicast Protocol */
#define NG_PROTNUM_PTP                  (123)   /**< Performance Transparency Protocol */
#define NG_PROTNUM_ISIS_OVER_IPV4       (124)   /**< ISIS over IPv4 */
#define NG_PROTNUM_FIRE                 (125)   /**< FIRE */
#define NG_PROTNUM_CRTP                 (126)   /**< Combat Radio Transport Protocol */
#define NG_PROTNUM_CRUDP                (127)   /**< Combat Radio User Datagram */
#define NG_PROTNUM_SSCOPMCE             (128)   /**< SSCOPMCE */
#define NG_PROTNUM_IPLT                 (129)   /**< IPLT */
#define NG_PROTNUM_SPS                  (130)   /**< Secure Packet Shield */
#define NG_PROTNUM_PIPE                 (131)   /**< Private IP Encapsulation within IP */
#define NG_PROTNUM_SCTP                 (132)   /**< Stream Control Transmission Protocol */
#define NG_PROTNUM_FC                   (133)   /**< Fibre Channel */
#define NG_PROTNUM_RSVP_E2E_IGNORE      (134)   /**< RSVP-E2E-IGNORE */
#define NG_PROTNUM_IPV6_EXT_MOB_HDR     (135)   /**< IPv6 Mobility Extension Header */
#define NG_PROTNUM_UDPLITE              (136)   /**< UDPLite */
#define NG_PROTNUM_MPLS_IN_IP           (137)   /**< MPLS-in-IP */
#define NG_PROTNUM_MANET                (138)   /**< MANET Protocols */
#define NG_PROTNUM_HIP                  (139)   /**< Host Identity Protocol */
#define NG_PROTNUM_SHIM6                (140)   /**< Shim6 Protocol */
#define NG_PROTNUM_WESP                 (141)   /**< Wrapped Encapsulating Security Payload */
#define NG_PROTNUM_ROHC                 (142)   /**< Robust Header Compression */
#define NG_PROTNUM_RESERVED             (255)   /**< Reserved */

#ifdef __cplusplus
}
#endif

#endif /* NG_PROTNUM_H_ */
/** @} */
