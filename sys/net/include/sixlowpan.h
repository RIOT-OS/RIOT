/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan 6LoWPAN
 * @ingroup     net
 * @brief       6LoWPAN module implements (parts of) the 6LoWPAN adaption layer
 *              for IPv6 over Low Power Wireless Personal Area Networks
 *              (6LoWPANs)
 *
 * @see     <a href="http://tools.ietf.org/html/rfc4919">
 *              RFC 4919 - IPv6 over Low-Power Wireless Personal Area
 *                  Networks (6LoWPANs): Overview, Assumptions, Problem
 *                  Statement, and Goals
 *          </a>
 * @see     <a href="http://tools.ietf.org/html/rfc4944">
 *              RFC 4944 - Transmission of IPv6 Packets over
 *                  IEEE 802.15.4 Networks
 *          </a>
 * @see     <a href="http://tools.ietf.org/html/rfc6282">
 *              RFC 6282 - Compression Format for IPv6 Datagrams over
 *                  IEEE 802.15.4-Based Networks
 *          </a>
 * @see     <a href="http://tools.ietf.org/html/rfc6775">
 *              RFC 6775 - Neighbor Discovery Optimization for IPv6
 *              over Low-Power Wireless Personal Area Networks
 *              (6LoWPANs)
 *          </a>
 *
 *
 * @{
 * @file        sixlowpan.h
 * @brief       6lowpan link layer and lowpan functions
 *
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SIXLOWPAN_H
#define SIXLOWPAN_H

#include "byteorder.h"
#include "cpu-conf.h"
#include "netapi.h"

#ifdef MODULE_SIXLOWPAN_HC
#include "sixlowpan/hc.h"
#endif /* MODULE_SIXLOWPAN_HC */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Recommended stack size for the 6LoWPAN control stack
 */
#define SIXLOWPAN_CONTROL_STACKSIZE     (KERNEL_CONF_STACKSIZE_DEFAULT)

/**
 * @brief   6LoWPAN dispatch value for uncompressed IPv6 packets.
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_IPV6_DISPATCH     (0x41)

/**
 * @brief   6LoWPAN dispatch value for fragmentation header (first fragment)
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_FRAG1_DISPATCH    (0xc0)

/**
 * @brief   6LoWPAN dispatch value for fragmentation header (subsequent fragment)
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_FRAGN_DISPATCH    (0xe0)

/**
 * @brief   6LoWPAN dispatch value for IPv6 header compression (part of
 *          first byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 4944, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_DISPATCH    (0x60)

/**
 * @brief   Mask to identify 6LoWPAN IPv6 header compression
 */
#define SIXLOWPAN_IPHC1_HDR_MASK    (0xe0)

/**
 * @brief   6LoWPAN fragmentation header length (first fragment)
 */
#define SIXLOWPAN_FRAG1_HDR_LEN     (4)

/**
 * @brief   6LoWPAN fragmentation header length (subsequent fragment)
 */
#define SIXLOWPAN_FRAGN_HDR_LEN     (5)

/**
 * @brief   Mask to identify 6LoWPAN fragmentation headers
 */
#define SIXLOWPAN_FRAG_HDR_MASK         (0xf8)

/**
 * @brief   Maximum possible length of a datagram compressed by 6LoWPAN
 *          fragmentation (2^11 - 1 == 0x7ff)
 */
#define SIXLOWPAN_FRAG_MAX_DATAGRAM_LEN (0x7ff)

/**
 * @brief   Data type to configure 6LoWPAN header compression according to
 *          <a href="https://tools.ietf.org/html/rfc6282">RFC 6282</a>.
 */
typedef enum __attribute__((packed)) {
    SIXLOWPAN_HC_DISABLE = 0,   /**< header compression disabled */
    SIXLOWPAN_HC_ENABLE = 1     /**< header compression enabled */
} sixlowpan_hc_status_t;

/**
 * @brief   Configuration type for the sixlowpan module
 *
 * @extends netapi_conf_type_t
 */
typedef enum {
    SIXLOWPAN_CONF_PROTO = NETAPI_CONF_PROTO,       /**< Get protocol of type netdev_proto_t.
                                                     *   Always NETDEV_PROTO_6LOWPAN
                                                     */

    SIXLOWPAN_CONF_CHANNEL = NETAPI_CONF_CARRIER,   /**< Get or set MAC layer channel */
    SIXLOWPAN_CONF_ADDRESS = NETAPI_CONF_ADDRESS,   /**< Get or set MAC layer address */
    SIXLOWPAN_CONF_PAN = NETAPI_CONF_SUBNETS,       /**< Get or set MAC layer PAN */
    SIXLOWPAN_CONF_MAX_PACKET_SIZE = NETAPI_CONF_MAX_PACKET_SIZE, /**< Get maximum packet size */
    SIXLOWPAN_CONF_ADDRESS_LONG = NETDEV_OPT_ADDRESS_LONG,  /** get or set alternative
                                                                MAC layer address */
    SIXLOWPAN_CONF_SRC_LEN = NETDEV_OPT_SRC_LEN,    /**< Get or set MAC default
                                                         source address mode */
    SIXLOWPAN_CONF_REGISTRY = NETAPI_CONF_REGISTRY, /**< Get or set 6LoWPAN receiver registry */

    /**
     * @brief   Get or set context of type sixlowpan_iphc_context_t on set and
     *          an array of sixlowpan_iphc_context_t on get. If
     *          sixlowpan_iphc_context_t::prefix_len is 0 the context will be
     *          deleted.
     */
    SIXLOWPAN_CONF_IPHC_CONTEXT,

    /**
     * @brief   Get or set compression status of type
     *          sixlowpan_hc_status_t
     */
    SIXLOWPAN_CONF_HC_STATUS,
} sixlowpan_conf_t;

/**
 * @brief   Set header compression status for 6LoWPAN.
 *
 * @param[in] pid       The PID of the 6LoWPAN thread
 * @param[in] status    Header compression status to set to.
 *
 * @return  netapi return code (0 on success)
 */
static inline int sixlowpan_hc_set_status(kernel_pid_t pid, sixlowpan_hc_status_t status)
{
#ifdef MODULE_SIXLOWPAN_HC
    return netapi_set_option(pid, (netapi_conf_type_t)SIXLOWPAN_CONF_HC_STATUS,
                             &status, sizeof(sixlowpan_hc_status_t));
#else
    (void)pid;
    (void)status;

    return -ENOTSUP;
#endif
}

/**
 * @brief   Get header compression status for 6LoWPAN.
 *
 * @param[in] pid       The PID of the 6LoWPAN thread
 *
 * @return  Header compression status.
 * @return  SIXLOWPAN_HC_DISABLE, on netapi error.
 */
sixlowpan_hc_status_t sixlowpan_hc_get_status(kernel_pid_t pid);

/**
 * @brief   Initialises the sixlowpan module. If it is already initialized
 *          it will just return the current PID of the control thread.
 *
 * @param[in] mac_pid   PID of the MAC layer
 * @param[in] stack     Stack to use for the control thread
 * @param[in] stacksize Maximum size of *stack*
 * @param[in] priority  Priority for the control thread
 * @param[in] flags     Flags for the control thread
 * @param[in] name      Name for the control thread
 *
 * @return PID of the control thread.
 */
kernel_pid_t sixlowpan_init(kernel_pid_t mac_pid, char *stack, int stacksize,
                            char priority, int flags, const char *name);

/**
 * @brief   Initializes a 6LoWPAN first fragment dispatch (length: 4 byte)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @param[out] data         Buffer to set the first 4 byte for
 * @param[in] datagram_size Size of the total datagram.
 *                          *datagram_size* is valid for *datagram_size* < 2^11
 * @param[in] datagram_tag  Tag for the datagram for identification
 */
static inline void sixlowpan_init_frag1_dispatch(uint8_t *data,
        uint16_t datagram_size, uint16_t datagram_tag)
{
    data[0] = (uint8_t)(SIXLOWPAN_FRAG1_DISPATCH | ((datagram_size & 0x0700) >> 8));
    data[1] = (uint8_t)(datagram_size & 0x00ff);
    data[2] = datagram_tag << 8;
    data[3] = (uint8_t)datagram_tag;
}

/**
 * @brief   Initializes a 6LoWPAN subsequent fragment dispatch (length: 5 byte)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @param[out] data             Buffer to set the first 5 byte for
 * @param[in] datagram_size     Size of the total datagram.
 *                              *datagram_size* is valid for *datagram_size* < 2^11
 * @param[in] datagram_tag      Tag for the datagram for identification
 * @param[in] datagram_offset   Offset of the fragment in the payload datagram
 *                              in units of 8 byte.
 */
static inline void sixlowpan_init_fragn_dispatch(uint8_t *data,
        uint16_t datagram_size, uint16_t datagram_tag, uint8_t datagram_offset)
{
    data[0] = (uint8_t)(SIXLOWPAN_FRAGN_DISPATCH | ((datagram_size & 0x0700) >> 8));
    data[1] = (uint8_t)(datagram_size & 0x00ff);
    data[2] = datagram_tag << 8;
    data[3] = (uint8_t)datagram_tag;
    data[4] = datagram_offset;
}

#ifdef TEST_SUITES
/**
 * @brief   Resets the reassembly buffer for testing purposes
 *
 * @note    **WARNING!!:** This can lead to memory leaks if you do not know
 *          what you are doing. Make sure the data in the reassembly buffer
 *          is freed first, then proceed with sixlowpan_reassembly_buf_reset()
 */
void sixlowpan_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SIXLOWPAN_H */
/** @} */
