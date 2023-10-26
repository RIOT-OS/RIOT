/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup posix
 *
 * @{
 * @file
 * @brief  posix netdb implementation
 * @author Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include "sys/socket.h"
#include "netinet/in.h"
#include "net/dns.h"

#include "netdb.h"

static const struct addrinfo default_hints =
{
    .ai_flags = 0,
    .ai_family = AF_UNSPEC,
    .ai_socktype = 0,
    .ai_protocol = 0,
    .ai_addrlen = 0,
    .ai_addr = NULL,
    .ai_canonname = NULL,
    .ai_next = NULL
};

typedef struct ipvx_addr {
    sa_family_t type;
    union {
        struct in_addr ipv4;
        struct in6_addr ipv6;
    };
} ipvx_addr_t;

void freeaddrinfo(struct addrinfo *ai)
{
    struct addrinfo *next;

    while (ai != NULL) {
        next = ai->ai_next;
        free(ai->ai_canonname);
        free(ai->ai_addr);
        free(ai);
        ai = next;
    }
}

int getaddrinfo(const char *nodename, const char *servname,
                const struct addrinfo *hints, struct addrinfo **res)
{
    ipvx_addr_t addr;
    int port = 0;
    size_t name_len = 0;

    if (res == NULL) {
        return EAI_FAIL;
    }
    else {
        *res = NULL;
    }

    if (nodename != NULL) {
        if ((nodename[0] == '\0') || (nodename[0] == '*' && nodename[1] == '\0')) {
            nodename = NULL;
        }
    }

    if (servname != NULL) {
        if ((servname[0] == '\0') || (servname[0] == '*' && servname[1] == '\0')) {
            servname = NULL;
        }
    }

    if ((nodename == NULL) && (servname == NULL)) {
        return EAI_NONAME;
    }

    if (nodename != NULL) {
        name_len = strlen(nodename);
        if (name_len > SOCK_DNS_MAX_NAME_LEN) {
            /* invalid name length */
            return EAI_FAIL;
        }
    }

    if (hints == NULL) {
        hints = &default_hints;
    }

    if (hints->ai_flags
            & ~(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV
                | AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG)
        ) {
        return EAI_BADFLAGS;
    }

    if ((hints->ai_flags & AI_CANONNAME) && nodename == NULL) {
        return EAI_BADFLAGS;
    }

    if ((hints->ai_family != AF_UNSPEC) &&
        (hints->ai_family != AF_INET) &&
        (hints->ai_family != AF_INET6)) {
        return EAI_FAMILY;
    }

    if (servname != NULL) { /* service location specified, try to resolve */
        if (hints->ai_flags & AI_NUMERICSERV) { /* service name specified: convert to port number */
            port = atoi(servname);

            if (port == 0 && (servname[0] != '0')) {
                /* atoi failed - service was not numeric */
                return EAI_SERVICE;
            }

            if ((port < 0) || (port > (int)0xffff)) {
                /* invalid port number */
                return EAI_SERVICE;
            }
        }
        else {
            /* non-numeric service names not supported */
            return EAI_SERVICE;
        }
    }

    if (nodename != NULL) { /* node location specified, try to resolve */
        if (hints->ai_flags & AI_NUMERICHOST) { /* no DNS lookup, just parse for an address string */

            if (hints->ai_family == AF_INET) {
                addr.type = AF_INET;
            }
            else if (hints->ai_family == AF_INET6) {
                addr.type = AF_INET6;
            }
            else { /* auto-detect IP version */
                if (strstr(nodename, ":") != NULL) { /* IPv6 */
                    addr.type = AF_INET6;
                }
                else if (strstr(nodename, ".") != NULL) { /* IPv4 */
                    addr.type = AF_INET;
                }
                else {
                    /* try IPv6 */
                    addr.type = AF_INET6;
                }
            }

            if (addr.type == AF_INET) {
                if (inet_pton(AF_INET, nodename, &addr.ipv4) != 1) {
                    return EAI_NONAME;
                }
            }
            else if (addr.type == AF_INET6) {
                if (inet_pton(AF_INET6, nodename, &addr.ipv6) != 1) {
                    return EAI_NONAME;
                }
            }
        }
        else { /* do actual DNS lookup */
            uint8_t addr_buf[16] = { 0 };
            int res = dns_query(nodename, addr_buf, hints->ai_family);
            switch (res) {
            case -ETIMEDOUT:
                /* fall-through */
            case -EAGAIN:
                return EAI_AGAIN;

            case -ENOMEM:
                return EAI_MEMORY;

            case INADDRSZ:
                addr.type = AF_INET;
                memcpy(&addr.ipv4, addr_buf, INADDRSZ);
                break;

            case IN6ADDRSZ:
                addr.type = AF_INET6;
                memcpy(&addr.ipv6, addr_buf, IN6ADDRSZ);
                break;

            default:
                return EAI_FAIL;
            }
        }
    }
    else { /* node location not specified, use loopback address */
        if (hints->ai_flags & AI_PASSIVE) {
            if (hints->ai_family == AF_INET6) {
                memcpy(&(addr.ipv6), &ipv6_addr_unspecified, sizeof(ipv6_addr_t));
            }
            else {
                memcpy(&(addr.ipv4), &ipv4_addr_any, sizeof(ipv4_addr_t));
            }
        }
        else {
            if (hints->ai_family == AF_INET6) {
                memcpy(&(addr.ipv6), &ipv6_addr_loopback, sizeof(ipv6_addr_t));
            }
            else {
                memcpy(&(addr.ipv4), &ipv4_addr_loopback, sizeof(ipv4_addr_t));
            }
        }
    }

    /* prepare return */

    struct addrinfo *ai;
    ai = (struct addrinfo*) calloc(1, sizeof(struct addrinfo));

    if (ai == NULL) {
        return EAI_MEMORY;
    }

    struct sockaddr_storage *sas;
    sas = (struct sockaddr_storage*) calloc(1, sizeof(struct sockaddr_storage));

    if (sas == NULL) {
        free(ai);
        return EAI_MEMORY;
    }

    if (addr.type == AF_INET) {
        ai->ai_family = AF_INET;

        struct sockaddr_in *sa4 = (struct sockaddr_in*) sas;
        sa4->sin_addr = addr.ipv4;
        sa4->sin_family = AF_INET;
        sa4->sin_port = htons(port);
    }
    else if (addr.type == AF_INET6) {
        ai->ai_family = AF_INET6;

        struct sockaddr_in6 *sa6 = (struct sockaddr_in6*) sas;
        sa6->sin6_addr = addr.ipv6;
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port = htons(port);
    }

    ai->ai_addr = (struct sockaddr*) sas;
    ai->ai_addrlen = sizeof(struct sockaddr_storage);
    ai->ai_socktype = hints->ai_socktype;
    ai->ai_protocol = hints->ai_protocol;

    if (nodename != NULL) {
        ai->ai_canonname = (char*) malloc(name_len + 1);

        if (ai->ai_canonname == NULL) {
            free(ai);
            free(sas);
            return EAI_MEMORY;
        }

        memcpy(ai->ai_canonname, nodename, name_len);
        ai->ai_canonname[name_len] = 0;
    }

    *res = ai;

    return 0;
}
