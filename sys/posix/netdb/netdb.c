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

#include "netdb.h"

struct hostent* gethostbyname(const char *name) {
#if LWIP_NETDB
    return lwip_gethostbyname(name);
#else
    #error "There is not implementation of gethostbyname available"
#endif
}

int gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                     size_t buflen, struct hostent **result, int *h_errnop) {
#if LWIP_NETDB
    return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop);
#else
    #error "There is not implementation of gethostbyname_r available"
#endif
}

void freeaddrinfo(struct addrinfo *ai)
{
#if LWIP_NETDB
    lwip_freeaddrinfo(ai);
#else
    #error "There is not implementation of freeaddrinfo available"
#endif
}

int getaddrinfo(const char *nodename, const char *servname,
            const struct addrinfo *hints, struct addrinfo **res)
{
#if LWIP_NETDB
    return lwip_getaddrinfo(nodename, servname, hints, res);
#else
    #error "There is not implementation of getaddrinfo available"
#endif
}
