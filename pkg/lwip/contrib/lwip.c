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
 */

#include "lwip/tcpip.h"
#include "lwip/netif/netdev2.h"
#include "lwip/netif.h"

#include "netdev2_tap.h"

#include "lwip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void lwip_bootstrap(void)
{
    tcpip_init(NULL, NULL);
}

/** @} */
