/*
 * Copyright (C) 2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to manage and show FIB Entries
 *
 * @author      2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "thread.h"
#include "inet_pton.h"
#include "net/af.h"
#ifdef MODULE_GNRC_NETIF
#include "net/gnrc/netif.h"
#endif
#include "net/fib.h"
#include "net/gnrc/ipv6.h"

#define INFO1_TXT "fibroute add <destination> via <next hop> [dev <device>]"
#define INFO2_TXT " [lifetime <lifetime>]"
#define INFO3_TXT "       <destination> - the destination address\n" \
                  "       <next hop>    - the address of the next-hop towards the <destination>\n" \
                  "       <device>      - the device id of the Interface to use." \
                  " Optional if only one interface is available.\n"
#define INFO4_TXT "       <lifetime>    - optional lifetime in ms when the entry automatically invalidates\n"
#define INFO5_TXT "fibroute del <destination>\n" \
                  "       <destination> - the destination address of the entry to be deleted\n"

static unsigned char tmp_ipv4_dst[INADDRSZ];  /**< buffer for ipv4 address conversion */
static unsigned char tmp_ipv4_nxt[INADDRSZ];  /**< buffer for ipv4 address conversion */
static unsigned char tmp_ipv6_dst[IN6ADDRSZ]; /**< buffer for ipv6 address conversion */
static unsigned char tmp_ipv6_nxt[IN6ADDRSZ]; /**< buffer for ipv6 address conversion */

static void _fib_usage(int info)
{
    switch (info) {
        case 1: {
            puts("\nbrief: adds a new entry to the FIB.\nusage: "
                 INFO1_TXT "\n" INFO3_TXT);
            break;
        }

        case 2: {
            puts("\nbrief: adds a new entry to the FIB.\nusage: "
                 INFO1_TXT INFO2_TXT "\n" INFO3_TXT INFO4_TXT);
            break;
        }

        case 3: {
            puts("\nbrief: deletes an entry from the FIB.\nusage: "
                 INFO5_TXT);
            break;
        }

        default:
            break;
    };
}

static void _fib_add(const char *dest, const char *next, kernel_pid_t pid, uint32_t lifetime)
{
    unsigned char *dst = (unsigned char *)dest;
    size_t dst_size = (strlen(dest));
    uint32_t dst_flags = 0xffff;
    unsigned char *nxt = (unsigned char *)next;
    size_t nxt_size = (strlen(next));
    uint32_t nxt_flags = 0xffff;

    /* determine destination address */
    if (inet_pton(AF_INET6, dest, tmp_ipv6_dst)) {
        dst = tmp_ipv6_dst;
        dst_size = IN6ADDRSZ;
        dst_flags = AF_INET6;
    }
    else if (inet_pton(AF_INET, dest, tmp_ipv4_dst)) {
        dst = tmp_ipv4_dst;
        dst_size = INADDRSZ;
        dst_flags = AF_INET;
    }

    /* determine next-hop address */
    if (inet_pton(AF_INET6, next, tmp_ipv6_nxt)) {
        nxt = tmp_ipv6_nxt;
        nxt_size = IN6ADDRSZ;
        nxt_flags = AF_INET6;
    }
    else if (inet_pton(AF_INET, next, tmp_ipv4_nxt)) {
        nxt = tmp_ipv4_nxt;
        nxt_size = INADDRSZ;
        nxt_flags = AF_INET;
    }

    fib_add_entry(&gnrc_ipv6_fib_table, pid, dst, dst_size, dst_flags, nxt,
                  nxt_size, nxt_flags, lifetime);
}

int _fib_route_handler(int argc, char **argv)
{
    /* e.g. fibroute right now dont care about the adress/protocol family */
    if (argc == 1) {
        fib_print_routes(&gnrc_ipv6_fib_table);
        return 0;
    }

    /* e.g. firoute [add|del] */
    if (argc == 2) {
        if ((strcmp("add", argv[1]) == 0)) {
            _fib_usage(2);
        }
        else if ((strcmp("del", argv[1]) == 0)) {
            _fib_usage(3);
        }
        else {
            puts("\nunrecognized parameter1.\nPlease enter fibroute [add|del] for more information.");
        }

        return 1;
    }

    if (argc > 2 && !((strcmp("add", argv[1]) == 0) || (strcmp("del", argv[1]) == 0))) {
        puts("\nunrecognized parameter2.\nPlease enter fibroute [add|del] for more information.");
        return 1;
    }

    /* e.g. fibroute del <destination> */
    if (argc == 3) {
        if (inet_pton(AF_INET6, argv[2], tmp_ipv6_dst)) {
            fib_remove_entry(&gnrc_ipv6_fib_table, tmp_ipv6_dst, IN6ADDRSZ);
        }
        else if (inet_pton(AF_INET, argv[2], tmp_ipv4_dst)) {
            fib_remove_entry(&gnrc_ipv6_fib_table, tmp_ipv4_dst, INADDRSZ);
        }
        else {
            fib_remove_entry(&gnrc_ipv6_fib_table, (uint8_t *)argv[2],
                             (strlen(argv[2])));
        }

        return 0;
    }

#ifdef MODULE_GNRC_NETIF
    /* e.g. fibroute add <destination> via <next hop> */
    if ((argc == 5) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t ifnum = gnrc_netif_get(ifs);
        if (ifnum == 1) {
            _fib_add(argv[2], argv[4], ifs[0], (uint32_t)FIB_LIFETIME_NO_EXPIRE);
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. fibroute add <destination> via <next hop> lifetime <lifetime> */
    if ((argc == 7) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("lifetime", argv[5]) == 0)) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t ifnum = gnrc_netif_get(ifs);
        if (ifnum == 1) {
            _fib_add(argv[2], argv[4], ifs[0], (uint32_t)atoi(argv[6]));
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }
#endif

    /* e.g. fibroute add <destination> via <next hop> dev <device> */
    if (argc == 7) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)) {
            _fib_add(argv[2], argv[4], (kernel_pid_t)atoi(argv[6]), (uint32_t)FIB_LIFETIME_NO_EXPIRE);
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. fibroute add <destination> via <next hop> dev <device> lifetime <lifetime> */
    if (argc == 9) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)
            && (strcmp("lifetime", argv[7]) == 0)) {
            _fib_add(argv[2], argv[4], (kernel_pid_t )atoi(argv[6]), (uint32_t)atoi(argv[8]));
        }
        else {
            _fib_usage(2);
            return 1;
        }

        return 0;
    }

    puts("\nunrecognized parameters.\nPlease enter fibroute [add|del] for more information.");
    return 1;
}
