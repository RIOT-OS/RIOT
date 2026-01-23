/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    tests_gnrc_ipv6_nib Common header for GNRC's NIB tests
 * @ingroup     tests
 * @brief       Common definitions for GNRC's NIB tests
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "net/gnrc.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _LL0            (0xce)
#define _LL1            (0xab)
#define _LL2            (0xfe)
#define _LL3            (0xad)
#define _LL4            (0xf7)
#define _LL5            (0x26)

extern gnrc_netif_t *_mock_netif;

void _tests_init(void);
int _mock_netif_get(gnrc_netapi_opt_t *opt);
void _common_set_up(void);

#ifdef __cplusplus
}
#endif

/** @} */
