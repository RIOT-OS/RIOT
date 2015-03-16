/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ipv6 IPv6
 * @ingroup     net
 * @brief       New IPv6 implementation
 *
 * The IPv6 control thread understands messages of type
 *
 *  * @ref NG_NETAPI_MSG_TYPE_RCV, and
 *  * @ref NG_NETAPI_MSG_TYPE_SND,
 *
 * @{
 *
 * @file
 * @brief       Definitions for IPv6
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_IPV6_H_
#define NG_IPV6_H_

#include "kernel_types.h"
#include "net/ng_netbase.h"
#include "thread.h"

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_ipv6/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the IPv6 thread
 */
#ifndef NG_IPV6_STACK_SIZE
#define NG_IPV6_STACK_SIZE (KERNEL_CONF_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the IPv6 thread
 */
#ifndef NG_IPV6_PRIO
#define NG_IPV6_PRIO            (PRIORITY_MAIN - 3)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread.
 */
#ifndef NG_IPV6_MSG_QUEUE_SIZE
#define NG_IPV6_MSG_QUEUE_SIZE  (8U)
#endif

/**
 * @brief   Initialization of the IPv6 thread.
 *
 * @return  The PID to the IPv6 thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 * @return  -EEXIST, if IPv6 was already initialized.
 */
kernel_pid_t ng_ipv6_init(void);

/**
 * @brief   Demultiplexes a packet according to @p nh.
 *
 * @internal
 *
 * **Do not use outside this module or its submodules!!!**
 * Public access needed for Extension Headers.
 *
 * @param[in] iface     The receiving interface.
 * @param[in] pkt       A packet.
 * @param[in] nh        A protocol number (see @ref net_ng_protnum).
 */
void ng_ipv6_demux(kernel_pid_t iface, ng_pktsnip_t *pkt, uint8_t nh);

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_H_ */
/**
 * @}
 */
