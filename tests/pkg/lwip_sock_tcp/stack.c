/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */
#include "lwip.h"
#include "lwip/netif.h"

#include "stack.h"

void _net_init(void)
{
    lwip_bootstrap();
}
