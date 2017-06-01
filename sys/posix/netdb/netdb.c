/*
 * Copyright(C) 2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @{
 *
 * @file
 *
 * @author  Francois Berder <francois.berder@imgtec.com>
 */

#include <netdb.h>
#include <sys/cdefs.h>
#include "net/sock/dns.h"
#include "net/sock/util.h"

#ifndef POSIX_DNS_SERVER
#error "POSIX_DNS_SERVER must be set"
#endif

#define str(s) #s
#define xstr(s) str(s)

sock_udp_ep_t sock_dns_server;

static char addr[16];
static char *addr_list[] = {addr, NULL};
static struct hostent dns_entry;

struct hostent* gethostbyname(const char *name)
{
    int res;
    sock_udp_str2ep(&sock_dns_server, xstr(POSIX_DNS_SERVER));

    res = sock_dns_query(name, addr, AF_UNSPEC);
    if (res <= 0)
        return NULL;

    memset(&dns_entry, 0, sizeof(dns_entry));

    if (res == 4) {
        dns_entry.h_addrtype = AF_INET;
        dns_entry.h_length = 4;
    } else {
        dns_entry.h_addrtype = AF_INET6;
        dns_entry.h_length = 16;
    }

    dns_entry.h_addr_list = addr_list;

    return &dns_entry;
}

/** @} */
