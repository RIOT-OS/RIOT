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

#define _LL0            (0xb8)
#define _LL1            (0x8c)
#define _LL2            (0xcc)
#define _LL3            (0xba)
#define _LL4            (0xef)
#define _LL5            (0x9a)
#define _LL6            (0x67)
#define _LL7            (0x42)

extern gnrc_netif_t *_mock_netif;

void _tests_init(void);
void _common_set_up(void);

#ifdef __cplusplus
}
#endif

/** @} */
