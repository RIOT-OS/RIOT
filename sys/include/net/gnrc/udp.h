/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_udp UDP
 * @ingroup     net_gnrc
 * @brief       GNRC's implementation of the UDP protocol
 *
 * @{
 *
 * @file
 * @brief       UDP GNRC definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_GNRC_UDP_H
#define NET_GNRC_UDP_H

#include <stdint.h>

#include "byteorder.h"
#include "net/gnrc.h"
#include "net/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_udp_conf  GNRC UDP compile configurations
 * @ingroup  net_gnrc_conf
 * @{
 */
/**
 * @brief   Default message queue size for the UDP thread (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_GNRC_UDP_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_UDP_MSG_QUEUE_SIZE_EXP (3U)
#endif

/**
 * @brief   Priority of the UDP thread
 */
#ifndef GNRC_UDP_PRIO
#define GNRC_UDP_PRIO           (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default stack size to use for the UDP thread
 *
 * @note    The message queue was previously allocated on the stack.
 *          The default number of messages is 2³.
 *          Given sizeof(msg_t) == 8, the stack size is reduced by 64 bytes.
 */
#ifndef GNRC_UDP_STACK_SIZE
#define GNRC_UDP_STACK_SIZE     ((THREAD_STACKSIZE_SMALL) - 64)
#endif
/** @} */

/**
 * @brief   Message queue size to use for the UDP thread.
 */
#ifndef GNRC_UDP_MSG_QUEUE_SIZE
#define GNRC_UDP_MSG_QUEUE_SIZE    (1 << CONFIG_GNRC_UDP_MSG_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   Calculate the checksum for the given packet
 *
 * @param[in] hdr           Pointer to the UDP header
 * @param[in] pseudo_hdr    Pointer to the network layer header
 *
 * @return  0 on success
 * @return  -EBADMSG if @p hdr is not of type GNRC_NETTYPE_UDP
 * @return  -EFAULT if @p hdr or @p pseudo_hdr is NULL
 * @return  -ENOENT if gnrc_pktsnip_t::type of @p pseudo_hdr is not known
 */
int gnrc_udp_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr);

/**
 * @brief   Allocate and initialize a fresh UDP header in the packet buffer
 *
 * @param[in] payload       Payload contained in the UDP packet
 * @param[in] src           Source port in host byte order
 * @param[in] dst           Destination port in host byte order
 *
 * @return  pointer to the newly created (and allocated) header
 * @return  NULL on `src == NULL`, `dst == NULL`, `src_len != 2`, `dst_len != 2`
 *          or on allocation error
 *
 * @pre     `src > 0` and `dst > 0`
 */
gnrc_pktsnip_t *gnrc_udp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src,
                                   uint16_t dst);

/**
 * @brief   Initialize and start UDP
 *
 * @return  PID of the UDP thread
 * @return  negative value on error
 */
int gnrc_udp_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_UDP_H */
