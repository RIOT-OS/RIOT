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
#ifndef NDP_H
#define NDP_H

#include <stdint.h>

#include "byteorder.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Router advertisement flags
 * @see     [RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2)
 * @{
 */
#define NDP_RTR_ADV_FLAGS_MASK      (0xc0)
#define NDP_RTR_ADV_FLAGS_M         (0x80)  /**< managed address configuration */
#define NDP_RTR_ADV_FLAGS_O         (0x40)  /**< other configuration */
/**
 * @}
 */

/**
 * @brief   Router advertisement constants
 * @see     [RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2)
 * @{
 */
#define NDP_RTR_ADV_CUR_HL_UNSPEC   (0) /**< current hop limit unspecified by advertising router */

/**
 * @brief   maximum router lifetime in seconds
 * @see     [RFC 4861, section 6.2.1](https://tools.ietf.org/html/rfc4861#section-6.2.1)
 */
#define NDP_RTR_ADV_LTIME_SEC_MAX   (9000)

/**
 * @brief   router should not be a default router
 * @see     [RFC 4861, section 6.2.1](https://tools.ietf.org/html/rfc4861#section-6.2.1)
 */
#define NDP_NBR_ADV_LTIME_NOT_DR    (0)

#define NDP_NBR_ADV_REACH_TIME      (0)     /**< reachable time unspecified by advertising router */
#define NDP_NBR_ADV_RETRANS_TIMER   (0)     /**< retrans. timer unspecified by advertising router */
/** @} */

/**
 * @name    Neighbor advertisement flags
 * @see     [RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2)
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
 * @see     [IANA, IPv6 Neighbor Discovery Option Formats]
 *          (http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-5")
 */
#define NDP_OPT_SL2A                (1)     /**< source link-layer address option */
#define NDP_OPT_TL2A                (2)     /**< target link-layer address option */
#define NDP_OPT_PI                  (3)     /**< prefix information option */
#define NDP_OPT_RH                  (4)     /**< redirected option */
#define NDP_OPT_MTU                 (5)     /**< MTU option */
#define NDP_OPT_AR                  (33)    /**< address registration option */
#define NDP_OPT_6CTX                (34)    /**< 6LoWPAN context option */
#define NDP_OPT_ABR                 (35)    /**< authoritative border router option */
/** @} */

/**
 * @{
 * @name    Flags for prefix information option
 */
#define NDP_OPT_PI_FLAGS_MASK       (0xc0)
#define NDP_OPT_PI_FLAGS_L          (0x80)  /**< on-link */
#define NDP_OPT_PI_FLAGS_A          (0x40)  /**< autonomous address configuration */
/** @} */

/**
 * @{
 * @name    Prefix information option constants
 * @see     [RFC 4861, section 4.6.2](https://tools.ietf.org/html/rfc4861#section-4.6.2)
 */
#define NDP_OPT_PI_VALID_LTIME_INF  (UINT32_MAX)    /**< infinite valid lifetime */
#define NDP_OPT_PI_PREF_LTIME_INF   (UINT32_MAX)    /**< infinite preferred lifetime */
/** @} */

/**
 * @{
 * @name    Lengths for fixed length options
 * @note    Options don't use bytes as their length unit, but 8 bytes.
 */
#define NDP_OPT_PI_LEN              (4U)
#define NDP_OPT_MTU_LEN             (1U)
/** @} */

/**
 * @{
 * @name    Router constants
 * @see     [RFC 4861, section 10](https://tools.ietf.org/html/rfc4861#section-10)
 */
#define NDP_MAX_INIT_RA_INTERVAL        (16000U)   /**< MAX_INITIAL_RTR_ADVERT_INTERVAL (in ms) */
#define NDP_MAX_INIT_RA_NUMOF           (3U)       /**< MAX_INITIAL_RTR_ADVERTISEMENT */
#define NDP_MAX_FIN_RA_NUMOF            (3U)       /**< MAX_FINAL_RTR_ADVERTISEMENT */
#define NDP_MIN_MS_DELAY_BETWEEN_RAS    (3000U)    /**< MIN_DELAY_BETWEEN_RAS (in ms) */
#define NDP_MAX_RA_DELAY                (500U)     /**< MAX_RA_DELAY_TIME (in ms) */
/** @} */

/**
 * @{
 * @name    Host constants
 * @see     [RFC 4861, section 10](https://tools.ietf.org/html/rfc4861#section-10)
 */
#define NDP_MAX_RS_MS_DELAY         (1000U)     /**< MAX_RTR_SOLICITATION_DELAY (in ms) */
#define NDP_RS_MS_INTERVAL          (4000U)     /**< RTR_SOLICITATION_INTERVAL (in ms) */
#define NDP_MAX_RS_NUMOF            (3U)        /**< MAX_RTR_SOLICITATIONS */
/** @} */

/**
 * @name    Node constants
 * @see     [RFC 4861, section 10](https://tools.ietf.org/html/rfc4861#section-10)
 * @{
 */
#define NDP_MAX_MC_SOL_NUMOF        (3U)        /**< MAX_MULTICAST_SOLICIT */
#define NDP_MAX_UC_SOL_NUMOF        (3U)        /**< MAX_UNICAST_SOLICIT */
#define NDP_MAX_ANYCAST_MS_DELAY    (1000U)     /**< MAX_ANYCAST_DELAY_TIME (in ms) */
#define NDP_MAX_NA_NUMOF            (3U)        /**< MAX_NEIGHBOR_ADVERTISEMENT */
#define NDP_REACH_MS                (30000U)    /**< REACHABLE_TIME (in ms) */
#define NDP_RETRANS_TIMER_MS        (1000U)     /**< RETRANS_TIMER (in ms) */
#define NDP_DELAY_FIRST_PROBE_MS    (5000U)     /**< DELAY_FIRST_PROBE_TIME (in ms) */
#define NDP_MIN_RANDOM_FACTOR       (500U)      /**< MIN_RANDOM_FACTOR (x 1000) */
#define NDP_MAX_RANDOM_FACTOR       (1500U)     /**< MAX_RANDOM_FACTOR (x 1000) */
/** @} */

/**
 * @brief   Router solicitation message format.
 * @extends icmpv6_hdr_t
 *
 * @see     [RFC 4861, section 4.1](https://tools.ietf.org/html/rfc4861#section-4.1)
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
 * @see     [RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2)
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
 * @see     [RFC 4861, section 4.3](https://tools.ietf.org/html/rfc4861#section-4.3)
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
 * @see     [RFC 4861, section 4.4](https://tools.ietf.org/html/rfc4861#section-4.4)
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
 * @brief   Redirect message format.
 * @extends icmpv6_hdr_t
 *
 * @see     [RFC 4861, section 4.5](https://tools.ietf.org/html/rfc4861#section-4.5)
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
 * @see     [RFC 4861, section 4.6](https://tools.ietf.org/html/rfc4861#section-4.6)
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
 * @see     [RFC 4861, section 4.6.2](https://tools.ietf.org/html/rfc4861#section-4.6.2)
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
 * @see     [RFC 4861, section 4.6.3](https://tools.ietf.org/html/rfc4861#section-4.6.3)
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
 * @see     [RFC 4861, section 4.6.4](https://tools.ietf.org/html/rfc4861#section-4.6.4)
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

#endif /* NDP_H */
/** @} */
