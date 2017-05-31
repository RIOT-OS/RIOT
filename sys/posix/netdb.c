/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @file    netdb.c
 * @brief   Providing implementation for prototypes defined in strings.h.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <inet_pton.h>

#include "destiny/socket.h"

#include "netdb.h"

void endhostent(void)
{
    // TODO
}

void endnetent(void)
{
    // TODO
}

void endprotoent(void)
{
    // TODO
}

void endservent(void)
{
    // TODO
}

void freeaddrinfo(struct addrinfo *ai)
{
    // TODO
}

const char *gai_strerror(int e)
{
    // TODO
    return "gai_strerror";
}

/**
 * Socket address type for IPv6 communication.
 */
struct sockaddr_in6 {
    uint8_t     sin6_family;    ///< set to AF_INET6
    uint16_t    sin6_port;      ///< transport layer port number
    uint32_t    sin6_flowinfo;  ///< IPv6 flow information
    ipv6_addr_t sin6_addr;      ///< IPv6 address
} __attribute__((packed));

int getaddrinfo(const char *host, const char *port,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
    static struct sockaddr_in6 sa;
    static struct addrinfo ai;
    ai.ai_addr = (struct sockaddr *) &sa;

    inet_pton(AF_INET6, host, &sa.sin6_addr);
    sa.sin6_family = AF_INET6;

    return 0; // success
}

struct hostent *gethostent(void)
{
    // TODO
    return NULL;
}
int getnameinfo(const struct sockaddr *host, socklen_t len,
                char *c, socklen_t d, char *e,
                socklen_t f, int g)
{
    // TODO
    return 0;
}

struct netent *getnetbyaddr(uint32_t a, int b)
{
    // TODO
    return NULL;
}

struct netent *getnetbyname(const char *name)
{
    // TODO
    return NULL;
}

struct netent *getnetent(void)
{
    // TODO
    return NULL;
}

struct protoent *getprotobyname(const char *name)
{
    // TODO
    return NULL;
}

struct protoent *getprotobynumber(int number)
{
    // TODO
    return NULL;
}

struct protoent *getprotoent(void)
{
    // TODO
    return NULL;
}

struct servent *getservbyname(const char *a, const char *b)
{
    // TODO
    return NULL;
}

struct servent *getservbyport(int a, const char *b)
{
    // TODO
    return NULL;
}

struct servent *getservent(void)
{
    // TODO
    return NULL;
}

void sethostent(int a)
{
    // TODO
}

void setnetent(int a)
{
    // TODO
}

void setprotoent(int a)
{
    // TODO
}

void setservent(int a)
{
    // TODO
}

/**
 * @}
 */
