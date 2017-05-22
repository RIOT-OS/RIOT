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

#ifdef __cplusplus
extern "C" {
#endif

#define _CALL(fn)   _common_set_up(); _set_up(); puts("Calling " # fn); fn()

extern kernel_pid_t _mock_netif_pid;

void _tests_init(void);
int _mock_netif_get(gnrc_netapi_opt_t *opt);
void _common_set_up(void);


#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
/** @} */
