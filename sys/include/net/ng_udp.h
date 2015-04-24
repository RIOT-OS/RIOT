/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_udp UDP
 * @ingroup     net
 * @brief       RIOT's implementation of the UDP protocol
 *
 * @{
 *
 * @file
 * @brief       UDP interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_UDP_H_
#define NG_UDP_H_

#include <stdint.h>

#include "byteorder.h"
#include "net/ng_netbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default message queue size for the UDP thread
 */
#ifndef NG_UDP_MSG_QUEUE_SIZE
#define NG_UDP_MSG_QUEUE_SIZE   (8U)
#endif

/**
 * @brief   Priority of the pktdump thread
 */
#ifndef NG_UDP_PRIO
#define NG_UDP_PRIO             (PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default stack size to use for the UDP thread
 */
#ifndef NG_UDP_STACK_SIZE
#define NG_UDP_STACK_SIZE       (KERNEL_CONF_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   UDP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;      /**< source port, in network byte order */
    network_uint16_t dst_port;      /**< destination port, network byte order */
    network_uint16_t length;        /**< payload length (including the header),
                                     *   network byte order */
    network_uint16_t checksum;      /**< checksum */
} ng_udp_hdr_t;

/**
 * @brief   Calculate the checksum for the given packet
 *
 * @param[in] hdr           Pointer to the UDP header
 * @param[in] pseudo_hdr    Pointer to the network layer header
 *
 * @return  0 on success
 * @return  -EBADMSG if @p pkt is not of type NG_NETTYPE_UDP
 * @return  -EFAULT if @p pkt or @p pseudo_hdr is NULL
 * @return  -ENOENT if @p pseudo_hdr_type is not known
 */
int ng_udp_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr);

/**
 * @brief   Allocate and initialize a fresh UDP header in the packet buffer
 *
 * @param[in] payload       Payload contained in the UDP packet
 * @param[in] src           Source port in host byte order
 * @param[in] src_len       Length of @p src, must be 2
 * @param[in] dst           Destination port in host byte order
 * @param[in] dst_len       Length of @p dst, must be 2
 *
 * @return  pointer to the newly created (and allocated) header
 * @return  NULL on error
 */
ng_pktsnip_t *ng_udp_hdr_build(ng_pktsnip_t *payload,
                               uint8_t *src, size_t src_len,
                               uint8_t *dst, size_t dst_len);

/**
 * @brief   Initialize and start UDP
 *
 * @return  PID of the UDP thread
 * @return  negative value on error
 */
int ng_udp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NG_UDP_H_ */
/** @} */
