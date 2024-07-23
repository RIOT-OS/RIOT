/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief   GNRC IPv6 Firewall
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include "net/gnrc/ipv6/firewall.h"

XFA_INIT_CONST(gnrc_ipv6_firewall_rule_t, gnrc_ipv6_firewall_rules_const);

#if CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
static gnrc_ipv6_firewall_rule_t rules[CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF];
#endif

static void _match_rule(const ipv6_hdr_t *hdr, const gnrc_ipv6_firewall_rule_t *rule,
                        uint8_t *prio, bool *match)
{
    if (!rule->flags) {
        return;
    }

    if (*prio > rule->prio) {
        return;
    }

    bool drop = rule->flags & GNRC_IPV6_FW_FLAG_MATCH_DROP;

    if (rule->flags & GNRC_IPV6_FW_FLAG_MATCH_SRC) {
        if (ipv6_addr_match_mask(&hdr->src, &rule->addr, &rule->mask)) {
            *match = drop;
            *prio = rule->prio;
        }
    }

    if (rule->flags & GNRC_IPV6_FW_FLAG_MATCH_DST) {
        if (ipv6_addr_match_mask(&hdr->dst, &rule->addr, &rule->mask)) {
            *match = drop;
            *prio = rule->prio;
        }
    }
}

bool gnrc_ipv6_firewall_is_blocked(const ipv6_hdr_t *hdr)
{
   bool drop = true;    /* if no rule matches, drop by default */
   uint8_t prio = 0;

   unsigned rules_const_numof = XFA_LEN(gnrc_ipv6_firewall_rule_t, gnrc_ipv6_firewall_rules_const);
   const gnrc_ipv6_firewall_rule_t *end = &gnrc_ipv6_firewall_rules_const[rules_const_numof];
   for (const gnrc_ipv6_firewall_rule_t *rule = &gnrc_ipv6_firewall_rules_const[0];
        rule != end;
        ++rule) {
        _match_rule(hdr, rule, &prio, &drop);
    }

#if CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
   for (const gnrc_ipv6_firewall_rule_t *rule = &rules[0];
        rule != &rules[CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF];
        ++rule) {
        _match_rule(hdr, rule, &prio, &drop);
    }
#endif

    return drop;
}

int gnrc_ipv6_firewall_add(const gnrc_ipv6_firewall_rule_t *new_rule)
{
#if CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
    for (gnrc_ipv6_firewall_rule_t *rule = &rules[0];
        rule != &rules[CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF];
        ++rule) {

        if (!rule->flags) {
            memcpy(rule, new_rule, sizeof(*rule));
            return 0;
        }
    }
#else
    (void)new_rule;
#endif

    return -ENOMEM;
}

void gnrc_ipv6_firewall_del_by_addr(const ipv6_addr_t *addr)
{
#if CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
    for (gnrc_ipv6_firewall_rule_t *rule = &rules[0];
        rule != &rules[CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF];
        ++rule) {

        if (!rule->flags) {
            continue;
        }

        if (ipv6_addr_equal(&rule->addr, addr)) {
            rule->flags = 0;
        }
    }
#else
    (void)addr;
#endif
}

void gnrc_ipv6_firewall_del_by_idx(unsigned idx)
{
    unsigned rules_const_numof = XFA_LEN(gnrc_ipv6_firewall_rule_t, gnrc_ipv6_firewall_rules_const);

    if (idx < rules_const_numof ||
       idx >= CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF + rules_const_numof) {
        return;
    }

    rules[idx].flags = 0;
}

static void _print_rule(unsigned idx, bool fix, const gnrc_ipv6_firewall_rule_t *rule)
{
    if (!rule->flags) {
        return;
    }

    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    const char *action = rule->flags & GNRC_IPV6_FW_FLAG_MATCH_DROP
                       ? "deny"
                       : "allow";

    printf("%u%s: %s %s%s %s", idx, fix ? "*" : "", action,
                             rule->flags & GNRC_IPV6_FW_FLAG_MATCH_SRC ? "from" : "",
                             rule->flags & GNRC_IPV6_FW_FLAG_MATCH_DST ? "to" :"",
                             ipv6_addr_to_str(addr_str, &rule->addr, sizeof(addr_str)));

    printf(" mask %s prio %u\n", ipv6_addr_to_str(addr_str, &rule->mask, sizeof(addr_str)),
                                 rule->prio);
}

void gnrc_ipv6_firewall_print(void)
{
    unsigned rules_const_numof = XFA_LEN(gnrc_ipv6_firewall_rule_t, gnrc_ipv6_firewall_rules_const);

    for (unsigned i = 0; i < rules_const_numof; ++i) {
        _print_rule(i, true, &gnrc_ipv6_firewall_rules_const[i]);
    }

#if CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_FIREWALL_RULES_NUMOF; ++i) {
        _print_rule(i + rules_const_numof, false, &rules[i]);
    }
#endif
}
