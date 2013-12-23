/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */
#include <netinet/in.h>

const struct sockaddr_in6 in6addr_any = {{AF_INET6, 0, 0, IN6ADDR_ANY_INIT}};
const struct sockaddr_in6 in6addr_loopback = {{
        AF_INET6, 0, 0, IN6ADDR_LOOPBACK_INIT
    }
};
