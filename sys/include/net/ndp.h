/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ndp IPv6 neighbor discovery
 * @ingroup     net_ipv6
 * @brief       Provides IPv6 neighbor discovery message types
 * @{
 *
 * @file
 * @brief       IPv6 neighbor discovery message type definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NDP_H_
#define NDP_H_

#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Router advertisement flags
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 * @{
 */
#define NDP_RTR_ADV_FLAGS_MASK      (0xc0)
#define NDP_RTR_ADV_FLAGS_M         (0x80)  /**< managed address configuration */
#define NDP_RTR_ADV_FLAGS_O         (0x40)  /**< other configuration */
/**
 * @}
 */

/**
 * @name    Neighbor advertisement flags
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.2
 *      </a>
 * @{
 */
#define NDP_NBR_ADV_FLAGS_MASK      (0xe0)
#define NDP_NBR_ADV_FLAGS_R         (0x80)  /**< router */
#define NDP_NBR_ADV_FLAGS_S         (0x40)  /**< solicited */
#define NDP_NBR_ADV_FLAGS_O         (0x20)  /**< override */
/**
 * @}
 */

/**
 * @name    NDP option types
 * @{
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-5">
 *          IANA, IPv6 Neighbor Discovery Option Formats
 *      </a>
 */
#define NDP_OPT_SL2A                (1)     /**< source link-layer address option */
#define NDP_OPT_TL2A                (2)     /**< target link-layer address option */
#define NDP_OPT_PI                  (3)     /**< prefix information option */
#define NDP_OPT_RH                  (4)     /**< redirected option */
#define NDP_OPT_MTU                 (5)     /**< MTU option */
/**
 * @}
 */

/**
 * @{
 * @name    Flags for prefix information option
 */
#define NDP_OPT_PI_FLAGS_MASK       (0xc0)
#define NDP_OPT_PI_FLAGS_L          (0x80)  /**< on-link */
#define NDP_OPT_PI_FLAGS_A          (0x40)  /**< autonomous address configuration */
/**
 * @}
 */

/**
 * @{
 * @name    Lengths for fixed length options
 * @brief   Options don't use bytes as their length unit, but 8 bytes.
 */
#define NDP_OPT_PI_LEN              (4U)
#define NDP_OPT_MTU_LEN             (1U)
/**
 * @}
 */

/**
 * @brief   Router solicitation message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t resv;  /**< reserved field */
} ndp_rtr_sol_t;

/**
 * @brief   Router advertisement message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                       /**< message type */
    uint8_t code;                       /**< message code */
    network_uint16_t csum;              /**< checksum */
    uint8_t cur_hl;                     /**< current hop limit */
    uint8_t flags;                      /**< flags */
    network_uint16_t ltime;             /**< router lifetime */
    network_uint32_t reach_time;        /**< reachable time */
    network_uint32_t retrans_timer;     /**< retransmission timer */
} ndp_rtr_adv_t;

/**
 * @brief   Neighbor solicitation message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t resv;  /**< reserved field */
    ipv6_addr_t tgt;        /**< target address */
} ndp_nbr_sol_t;

/**
 * @brief   Neighbor advertisement message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint8_t flags;          /**< flags */
    uint8_t resv[3];        /**< reserved fields */
    ipv6_addr_t tgt;        /**< target address */
} ndp_nbr_adv_t;

/**
 * @brief   Neighbor advertisement message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.5">
 *          RFC 4861, section 4.5
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t resv;  /**< reserved field */
    ipv6_addr_t tgt;        /**< target address */
    ipv6_addr_t dst;        /**< destination address */
} ndp_redirect_t;

/**
 * @brief   General NDP option format
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6">
 *          RFC 4861, section 4.6
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;   /**< option type */
    uint8_t len;    /**< length in units of 8 octets */
} ndp_opt_t;

/* XXX: slla and tlla are just ndp_opt_t with variable link layer address
 * appended */

/**
 * @brief   Prefix information option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                   /**< option type */
    uint8_t len;                    /**< length in units of 8 octets */
    uint8_t prefix_len;             /**< prefix length */
    uint8_t flags;                  /**< flags */
    network_uint32_t valid_ltime;   /**< valid lifetime */
    network_uint32_t pref_ltime;    /**< preferred lifetime */
    network_uint32_t resv;          /**< reserved field */
    ipv6_addr_t prefix;             /**< prefix */
} ndp_opt_pi_t;

/**
 * @brief   Redirected header option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.3">
 *          RFC 4861, section 4.6.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    uint8_t resv[6];        /**< reserved field */
} ndp_opt_rh_t;

/**
 * @brief   MTU option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    network_uint16_t resv;  /**< reserved field */
    network_uint32_t mtu;   /**< MTU */
} ndp_opt_mtu_t;


#ifdef __cplusplus
}
#endif

#endif /* NDP_H_ */
/** @} */
