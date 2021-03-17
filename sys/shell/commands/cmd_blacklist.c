/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#include <stdio.h>
#include <string.h>

#include "net/gnrc/ipv6/blacklist.h"

static void _usage(char *cmd)
{
    printf("usage: * %s\n", cmd);
    puts("         Lists all addresses in the blacklist.");
    printf("       * %s add <addr>\n", cmd);
    puts("         Adds <addr> to the blacklist.");
    printf("       * %s del <addr>\n", cmd);
    puts("         Deletes <addr> from the blacklist.");
    printf("       * %s help\n", cmd);
    puts("         Print this.");
}

int _blacklist(int argc, char **argv)
{
    ipv6_addr_t addr;
    if (argc < 2) {
        gnrc_ipv6_blacklist_print();
        return 0;
    }
    else if (argc > 2) {
        if (ipv6_addr_from_str(&addr, argv[2]) == NULL) {
            _usage(argv[0]);
            return 1;
        }
    }
    if (strcmp("add", argv[1]) == 0) {
        gnrc_ipv6_blacklist_add(&addr);
    }
    else if (strcmp("del", argv[1]) == 0) {
        gnrc_ipv6_blacklist_del(&addr);
    }
    else if (strcmp("help", argv[1]) == 0) {
        _usage(argv[0]);
    }
    else {
        _usage(argv[0]);
        return 1;
    }
    return 0;
}

/** @} */
