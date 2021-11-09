/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lwip_opts   lwIP options
 * @ingroup     pkg_lwip
 * @brief       Options for the lwIP stack
 * @{
 *
 * @file
 * @brief   Option definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

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
#if defined(MODULE_LWIP_ARP) && defined(MODULE_LWIP_ETHERNET)
#define LWIP_ARP                1
#else  /* MODULE_LWIP_ARP */
#define LWIP_ARP                0
#endif /* MODULE_LWIP_ARP */

#ifdef MODULE_LWIP_AUTOIP
#define LWIP_AUTOIP             1
#else  /* MODULE_LWIP_AUTOIP */
#define LWIP_AUTOIP             0
#endif /* MODULE_LWIP_AUTOIP */

#ifdef MODULE_LWIP_DHCP
#define LWIP_DHCP               1
#else  /* MODULE_LWIP_DHCP */
#define LWIP_DHCP               0
#endif /* MODULE_LWIP_DHCP */

#ifdef MODULE_LWIP_ETHERNET
#define LWIP_ETHERNET           1
#else  /* MODULE_LWIP_ETHERNET */
#define LWIP_ETHERNET           0
#endif /* MODULE_LWIP_ETHERNET */

#ifdef MODULE_LWIP_NETIF
#define LWIP_NETIF_API          1
#else  /* MODULE_LWIP_NETIF */
#define LWIP_NETIF_API          0
#endif /* MODULE_LWIP_NETIF */

#ifdef MODULE_LWIP_IGMP
#define LWIP_IGMP               1
#else  /* MODULE_LWIP_IGMP */
#define LWIP_IGMP               0
#endif /* MODULE_LWIP_IGMP */

#ifdef MODULE_LWIP_IPV4
#define LWIP_IPV4               1
#else  /* MODULE_LWIP_IPV4 */
#define LWIP_IPV4               0
#endif /* MODULE_LWIP_IPV4 */

#ifdef MODULE_LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG    1
#else  /* MODULE_LWIP_IPV6_AUTOCONFIG */
#define LWIP_IPV6_AUTOCONFIG    0
#endif /* MODULE_LWIP_IPV6_AUTOCONFIG */

#ifdef MODULE_LWIP_IPV6_MLD
#define LWIP_IPV6_MLD           1
#else  /* MODULE_LWIP_IPV6 */
#define LWIP_IPV6_MLD           0
#endif /* MODULE_LWIP_IPV6 */

#ifdef MODULE_LWIP_IPV6
#define LWIP_IPV6               1
#else  /* MODULE_LWIP_IPV6 */
#define LWIP_IPV6               0
#endif /* MODULE_LWIP_IPV6 */

#ifdef MODULE_LWIP_NETIF_PPP
#define PPP_SUPPORT             1
#else  /* MODULE_LWIP_NETIF_PPP */
#define PPP_SUPPORT             0
#endif /* MODULE_LWIP_NETIF_PPP */

#ifdef MODULE_LWIP_RAW
#define LWIP_RAW                1
#else  /* MODULE_LWIP_RAW */
#define LWIP_RAW                0
#endif /* MODULE_LWIP_RAW */

#ifdef MODULE_LWIP_SIXLOWPAN
#define LWIP_6LOWPAN            1
#else  /* MODULE_LWIP_STATS */
#define LWIP_6LOWPAN            0
#endif /* MODULE_LWIP_STATS */

#ifdef MODULE_LWIP_STATS
#define LWIP_STATS              1
#else  /* MODULE_LWIP_STATS */
#define LWIP_STATS              0
#endif /* MODULE_LWIP_STATS */

#ifdef MODULE_LWIP_TCP
#define LWIP_TCP                1
#else  /* MODULE_LWIP_TCP */
#define LWIP_TCP                0
#endif /* MODULE_LWIP_TCP */

#ifdef MODULE_LWIP_UDP
#define LWIP_UDP                1
#else  /* MODULE_LWIP_UDP */
#define LWIP_UDP                0
#endif /* MODULE_LWIP_UDP */

#ifdef MODULE_LWIP_UDPLITE
#define LWIP_UDPLITE            1
#else  /* MODULE_LWIP_UDPLITE */
#define LWIP_UDPLITE            0
#endif /* MODULE_LWIP_UDPLITE */

#if IS_USED(MODULE_LWIP_SOCK)
#define LWIP_NETCONN            1
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
#define LWIP_NETBUF_RECVINFO    1
#endif /* MODULE_SOCK_AUX_LOCAL */
#else
#define LWIP_NETCONN            0
#endif /* MODULE_LWIP_SOCK */

#ifdef MODULE_SHELL_COMMANDS
#define LWIP_DEBUG              1
#endif

#ifndef TCP_LISTEN_BACKLOG
# if defined(MODULE_LWIP_SOCK_TCP)
# define TCP_LISTEN_BACKLOG     1
# else
# define TCP_LISTEN_BACKLOG     0
# endif
#endif /* TCP_LISTEN_BACKLOG */

#define LWIP_SOCKET             0

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#define MEMP_MEM_MALLOC         1
#define NETIF_MAX_HWADDR_LEN    (GNRC_NETIF_HDR_L2ADDR_MAX_LEN)

#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)
#endif

#define MEM_ALIGNMENT           4
#ifndef MEM_SIZE
/* packet buffer size of GNRC + stack for TCP/IP */
#define MEM_SIZE                (TCPIP_THREAD_STACKSIZE + 6144)
#endif

#ifdef DEVELHELP
void sys_mark_tcpip_thread(void);
#define LWIP_MARK_TCPIP_THREAD sys_mark_tcpip_thread

bool sys_check_core_locked(void);
#define LWIP_ASSERT_CORE_LOCKED() \
    LWIP_ASSERT("Core lock held", sys_check_core_locked())
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LWIPOPTS_H */
/** @} */
