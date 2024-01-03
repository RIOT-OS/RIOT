/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_firewall minimal IPv6 firewall
 * @ingroup     net_gnrc_ipv6
 * @brief       This allows to block/allow forwarding to certain
 *              addresses/ranges
 * @{
 *
 * @file
 * @brief   IPv6 firewall definitions
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NET_GNRC_IPV6_FIREWALL_H
#define NET_GNRC_IPV6_FIREWALL_H

#include <stdbool.h>
#include "net/ipv6/hdr.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of dynamic firewall rules
 */
#ifndef CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
#define CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF (4)
#endif

#define GNRC_IPV6_FW_FLAG_MATCH_DROP (1U << 0)  /**< Packet is dropped if rule matches */
#define GNRC_IPV6_FW_FLAG_MATCH_SRC (1U << 1)   /**< Rule applies to source address */
#define GNRC_IPV6_FW_FLAG_MATCH_DST (1U << 2)   /**< Rule applies to destination address */

/**
 * @brief   Add a compile-time const firewall rule
 */
#define GNRC_IPV6_FIREWALL_ADD(name)             \
    XFA_CONST(gnrc_ipv6_firewall_rules_const, 0) \
        gnrc_ipv6_firewall_rule_t (name)

/**
 * @brief Firewall rule struct
 */
typedef struct {
    ipv6_addr_t addr;   /**< address to match    */
    ipv6_addr_t mask;   /**< bitmask for address */
    uint8_t prio;       /**< rule priority       */
    uint8_t flags;      /**< rule flags          */
    uint8_t padding[2]; /**< padding for XFA     */
} gnrc_ipv6_firewall_rule_t;

/**
 * @brief   Check if a packet is blocked by firewall rules
 *
 * @param[in] hdr   IPv6 header of the received packet
 *
 * @return  true if the packet should not be forwarded
 */
bool gnrc_ipv6_firewall_is_blocked(const ipv6_hdr_t *hdr);

/**
 * @brief   Add a rule to the firewall
 *
 * @param[in] rule  Firewall rule to add
 *
 * @return  0 on success, error otherwise
 */
int gnrc_ipv6_firewall_add(const gnrc_ipv6_firewall_rule_t *rule);

/**
 * @brief   Delete a firewall rule referenced by index
 * @param[in] idx   Index of the rule to delete
 */
void gnrc_ipv6_firewall_del_by_idx(unsigned idx);

/**
 * @brief   Print all firewall rules to stdout
 */
void gnrc_ipv6_firewall_print(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_FIREWALL_H */
/** @} */
