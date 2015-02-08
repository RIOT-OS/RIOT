/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    netinet_in.c
 * @brief   Providing values for in6addr_any and in6addr_loopback.
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <netinet/in.h>

const struct sockaddr_in6 in6addr_any = {{AF_INET6, 0, 0, IN6ADDR_ANY_INIT}, 0};
const struct sockaddr_in6 in6addr_loopback = {{
        AF_INET6, 0, 0, IN6ADDR_LOOPBACK_INIT
    }, 0
};

/**
 * @}
 */
