/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineernig-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands.h
 * @{
 *
 * @file
 *
 * @author      Nick van IJzendoorn <nijzednoorn@engineering-spirit.nl>
 */

#include <string.h>
#include <stdio.h>
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/dns.h"

typedef enum {
    Add,
    Remove
} dns_mod_action_t;

int _gnrc_dns_resolve(const char *arg1, const char *arg2) {
    gnrc_dns_type_t type = GNRC_DNS_TYPE_AAAA;
    gnrc_dns_response_t response;

    /* check request type */
    if (!arg2 || strcasecmp(arg2, "AAAA") == 0) {
        type = GNRC_DNS_TYPE_AAAA;
    } else if (strcasecmp(arg2, "A") == 0) {
        type = GNRC_DNS_TYPE_A;
    } else if (strcasecmp(arg2, "SRV") == 0) {
        type = GNRC_DNS_TYPE_SRV;
    } else if (strcasecmp(arg2, "TXT") == 0) {
        type = GNRC_DNS_TYPE_TXT;
    }

    /* resolve the given domain name */
    if (gnrc_dns_resolve(arg1, type, &response)) {
        char ip_buffer[IPV6_ADDR_MAX_STR_LEN];

        printf("successfully resolved domain: %s\n", arg1);

        printf(" - full name: %s\n\n", response.full_name);

        switch (response.type) {
        case GNRC_DNS_TYPE_A:
            for (int idx = 0; idx < response.responses; ++idx)
#ifdef MODULE_IPV4
                printf(" - %s\n",
                        ipv4_addr_to_str(ip_buffer, response.data.a + idx,
                        sizeof(ip_buffer)));
#else
                puts(" - no IPv4 support");
#endif
            break;

        case GNRC_DNS_TYPE_AAAA:
            for (int idx = 0; idx < response.responses; ++idx)
                printf(" - %s\n",
                        ipv6_addr_to_str(ip_buffer, response.data.aaaa + idx,
                        sizeof(ip_buffer)));
            break;

        case GNRC_DNS_TYPE_TXT:
            printf("%s\n", response.data.txt);
            break;

        case GNRC_DNS_TYPE_SRV:
            for (int idx = 0; idx < response.responses; ++idx)
                printf(" - p:%02" PRIu16 " w:%02" PRIu16 ": %s:%" PRIu16 "\n",
                        response.data.srv[idx].priority,
                        response.data.srv[idx].weight,
                        response.data.srv[idx].target,
                        response.data.srv[idx].port);
            break;

        default:
            break;
        }
    } else {
        switch (response.type) {
        case GNRC_DNS_ERROR_TIMEDOUT:
            puts("error: request timed out.");
            break;

        case GNRC_DNS_ERROR_NO_ENTRY:
            puts("error: domain name does not exist.");
            break;

        case GNRC_DNS_ERROR_MSG:
            puts("error: error message received.");
            break;

        default:
            break;
        }

        puts("error: failed to resolve DNS name.");
    }

    return 0;
}

static int _gnrc_dns_print(void) {
    int idx = 0;
    gnrc_dns_server_info_t info;
    char _ipv6_addr[IPV6_ADDR_MAX_STR_LEN];

    /* print the information header */
    puts("Available DNS servers:");
    printf("# | %-*s | ttl\n", IPV6_ADDR_MAX_STR_LEN, "address");
    puts("---------------------------------------------------------------");

    /* print all DNS servers */
    while (gnrc_dns_get_server(idx, &info)) {
        printf("%d | %-*s | %" PRIu32 "\n", idx, IPV6_ADDR_MAX_STR_LEN,
                ipv6_addr_to_str(_ipv6_addr, &info.ip, sizeof(_ipv6_addr)),
                info.ttl);

        ++idx;
    }

    puts("");

    return 0;
}

static int _gnrc_dns_modify(const char *arg, dns_mod_action_t action) {
    ipv6_addr_t addr;

    /* check if we can parse the IP address */
    if (!ipv6_addr_from_str(&addr, arg)) {
        puts("error: couldn't parse IP address");
    }

    /* add or remove the given IP addres from the server collectiond */
    if (action == Add) {
        gnrc_dns_add_server(&addr);
    } else if (action == Remove) {
        gnrc_dns_del_server(&addr);
    } else {
        puts("error: unknown DNS server action");
    }

    return 0;
}

int _gnrc_dns_handler(int argc, char **argv) {
    if ((argc < 2) || (strcmp(argv[1], "show") == 0)) {
        return _gnrc_dns_print();
    } else if ((argc == 3) && strcmp(argv[1], "add") == 0) {
        return _gnrc_dns_modify(argv[2], Add);
    } else if ((argc == 3) && strcmp(argv[1], "del") == 0) {
        return _gnrc_dns_modify(argv[2], Remove);
    } else if ((argc >= 3) && strcmp(argv[1], "res") == 0) {
        return _gnrc_dns_resolve(argv[2], argv[3]);
    }

    printf("usage: %s [show|add|del|resolve]\n", argv[0]);
    puts("* help\t\t\t- show usage");
    puts("* show\t\t\t- show the current available DNS servers");
    puts("* add <addr>\t- add the DNS server to the list");
    puts("* del <addr>\t- remove the DNS server form the list");
    puts("* res <addr> <type:AAAA>\t\t- resolve the given domain name");
    return 0;
}

/**
 * @}
 */
