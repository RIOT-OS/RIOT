/*
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 *
 * @{
 *
 * @file
 * @brief       nanocoap options sort functions
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#ifndef NET_NANOCOAP_OPT_SORT_H
#define NET_NANOCOAP_OPT_SORT_H

#include <stdbool.h>

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Test that options are sorted by option number
 *
 * @param[in]  pkt     structure referencing options
 *
 * @returns    true if sorted
 */
bool coap_opt_sorted(coap_pkt_t *pkt);

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_OPT_SORT_H */
/** @} */
