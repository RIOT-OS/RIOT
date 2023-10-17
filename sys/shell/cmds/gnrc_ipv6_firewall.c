/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/gnrc/ipv6/firewall.h"
#include "shell.h"

static void _usage(char *cmd)
{
    printf("usage: %s <deny|allow> <to|from> <addr> [mask <mask>] [prio <prio>]\n", cmd);
    printf("       %s del <idx>\n", cmd);
    puts("\nadd a rule or delete a rule");
}

static int _parse_addr(gnrc_ipv6_firewall_rule_t *rule, const char *addr)
{
    int pfx_len;
    if (strrchr(addr, '/')) {
        pfx_len = ipv6_prefix_from_str(&rule->addr, addr);
        if (pfx_len < 0) {
            return 1;
        }
    } else {
        pfx_len = 128;
        if (ipv6_addr_from_str(&rule->addr, addr) == NULL) {
            return 1;
        }
    }

    uint8_t bytes = pfx_len / 8;
    uint8_t bits = pfx_len % 8;

    memset(&rule->mask, 0xff, pfx_len / 8);
    rule->mask.u8[bytes] = 0xff << (8 - bits);

    return 0;
}

static int _parse_rule(int argc, char **argv, gnrc_ipv6_firewall_rule_t *rule)
{
    while (argc > 1) {
        if (strcmp("from", argv[0]) == 0) {
            rule->flags |= GNRC_IPV6_FW_FLAG_MATCH_SRC;
            if (_parse_addr(rule, argv[1])) {
                return 1;
            }
        } else if (strcmp("to", argv[0]) == 0) {
            rule->flags |= GNRC_IPV6_FW_FLAG_MATCH_DST;
            if (_parse_addr(rule, argv[1])) {
                return 1;
            }
        } else if (strcmp("mask", argv[0]) == 0) {
            if (ipv6_addr_from_str(&rule->mask, argv[1]) == NULL) {
                return 1;
            }
        } else if (strcmp("prio", argv[0]) == 0) {
            rule->prio = atoi(argv[1]);
        } else {
            break;
        }

        argv += 2;
        argc -= 2;
    }

    return argc;
}

static int _cmd_firewall(int argc, char **argv)
{
    gnrc_ipv6_firewall_rule_t rule = {
        .prio = 0,
    };

    if (argc < 2) {
        gnrc_ipv6_firewall_print();
        return 0;
    }

    if (strcmp("del", argv[1]) == 0 && argc > 2) {
        unsigned idx = atoi(argv[2]);
        gnrc_ipv6_firewall_del_by_idx(idx);
        return 0;
    } else if (strcmp("deny", argv[1]) == 0 && argc > 3) {
        rule.flags |= GNRC_IPV6_FW_FLAG_MATCH_DROP;
        if (_parse_rule(argc - 2, argv + 2, &rule)) {
            puts("parse error");
            return 1;
        }
        gnrc_ipv6_firewall_add(&rule);
    } else if (strcmp("allow", argv[1]) == 0 && argc > 3) {
        if (_parse_rule(argc - 2, argv + 2, &rule)) {
            puts("parse error");
            return 1;
        }
        gnrc_ipv6_firewall_add(&rule);
    } else if (strcmp("help", argv[1]) == 0) {
        _usage(argv[0]);
    } else {
        _usage(argv[0]);
        return 1;
    }
    return 0;
}

SHELL_COMMAND(firewall, "set up basic forwarding rules", _cmd_firewall);
/** @} */
