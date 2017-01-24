/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    lwip_opts   lwIP options
 * @ingroup     lwip
 * @brief       Options for the lwIP stack
 * @{
 *
 * @file
 * @brief   Option definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

#include "thread.h"
#include "net/gnrc/netif/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   lwIP configuration macros.
 * @see     lwIP documentation
 * @{
 */
#ifdef MODULE_LWIP_ARP
#define LWIP_ARP                (1)
#else  /* MODULE_LWIP_ARP */
#define LWIP_ARP                (0)
#endif /* MODULE_LWIP_ARP */

#ifdef MODULE_LWIP_AUTOIP
#define LWIP_AUTOIP             (1)
#else  /* MODULE_LWIP_AUTOIP */
#define LWIP_AUTOIP             (0)
#endif /* MODULE_LWIP_AUTOIP */

#ifdef MODULE_LWIP_DHCP
#define LWIP_DHCP               (1)
#else  /* MODULE_LWIP_DHCP */
#define LWIP_DHCP               (0)
#endif /* MODULE_LWIP_DHCP */

#ifdef MODULE_LWIP_ETHERNET
#define LWIP_ETHERNET           (1)
#else  /* MODULE_LWIP_ETHERNET */
#define LWIP_ETHERNET           (0)
#endif /* MODULE_LWIP_ETHERNET */

#ifdef MODULE_LWIP_IGMP
#define LWIP_IGMP               (1)
#else  /* MODULE_LWIP_IGMP */
#define LWIP_IGMP               (0)
#endif /* MODULE_LWIP_IGMP */

#ifdef MODULE_LWIP_IPV4
#define LWIP_IPV4               (1)
#else  /* MODULE_LWIP_IPV4 */
#define LWIP_IPV4               (0)
#endif /* MODULE_LWIP_IPV4 */

#ifdef MODULE_LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG    (1)
#else  /* MODULE_LWIP_IPV6_AUTOCONFIG */
#define LWIP_IPV6_AUTOCONFIG    (0)
#endif /* MODULE_LWIP_IPV6_AUTOCONFIG */

#ifdef MODULE_LWIP_IPV6_MLD
#define LWIP_IPV6_MLD           (1)
#else  /* MODULE_LWIP_IPV6 */
#define LWIP_IPV6_MLD           (0)
#endif /* MODULE_LWIP_IPV6 */

#ifdef MODULE_LWIP_IPV6
#define LWIP_IPV6               (1)
#else  /* MODULE_LWIP_IPV6 */
#define LWIP_IPV6               (0)
#endif /* MODULE_LWIP_IPV6 */


#ifdef MODULE_LWIP_NETIF_PPP
#define PPP_SUPPORT             (1)
#else  /* MODULE_LWIP_NETIF_PPP */
#define PPP_SUPPORT             (0)
#endif /* MODULE_LWIP_NETIF_PPP */

#ifdef MODULE_LWIP_RAW
#define LWIP_RAW                (1)
#else  /* MODULE_LWIP_RAW */
#define LWIP_RAW                (0)
#endif /* MODULE_LWIP_RAW */

#ifdef MODULE_LWIP_SIXLOWPAN
#define LWIP_6LOWPAN            (1)
#else  /* MODULE_LWIP_STATS */
#define LWIP_6LOWPAN            (0)
#endif /* MODULE_LWIP_STATS */

#ifdef MODULE_LWIP_STATS
#define LWIP_STATS              (1)
#else  /* MODULE_LWIP_STATS */
#define LWIP_STATS              (0)
#endif /* MODULE_LWIP_STATS */

#ifdef MODULE_LWIP_TCP
#define LWIP_TCP                (1)
#else  /* MODULE_LWIP_TCP */
#define LWIP_TCP                (0)
#endif /* MODULE_LWIP_TCP */

#ifdef MODULE_LWIP_UDP
#define LWIP_UDP                (1)
#else  /* MODULE_LWIP_UDP */
#define LWIP_UDP                (0)
#endif /* MODULE_LWIP_UDP */

#ifdef MODULE_LWIP_UDPLITE
#define LWIP_UDPLITE            (1)
#else  /* MODULE_LWIP_UDPLITE */
#define LWIP_UDPLITE            (0)
#endif /* MODULE_LWIP_UDPLITE */

#if defined(MODULE_LWIP_CONN) || defined(MODULE_LWIP_SOCK)
#define LWIP_NETCONN            (1)
#else
#define LWIP_NETCONN            (0)
#endif

#define LWIP_SOCKET             (0)
#define MEMP_MEM_MALLOC         (1)
#define NETIF_MAX_HWADDR_LEN    (GNRC_NETIF_HDR_L2ADDR_MAX_LEN)

#define TCPIP_THREAD_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)

#define MEM_ALIGNMENT           (4)
#ifndef MEM_SIZE
/* packet buffer size of GNRC + stack for TCP/IP */
#define MEM_SIZE                (TCPIP_THREAD_STACKSIZE + 6144)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_LWIPOPTS_H */
/** @} */
