/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef COMMON_H
#define COMMON_H

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

#endif /* COMMON_H */
/** @} */
