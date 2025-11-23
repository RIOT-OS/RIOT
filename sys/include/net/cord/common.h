/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_cord_common CoRE RD Common
 * @ingroup     net_cord
 * @brief       Shared functionality for CoRE Resource Directory endpoints
 * @{
 *
 * @file
 * @brief       Shared CoRE RD endpoint functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "net/cord/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Export the local endpoint identifier
 *
 * @note    Use cord_common_get_ep() for accessing the endpoint identifier
 */
extern char cord_common_ep[];

/**
 * @brief   Generate unique endpoint identifier (ep)
 */
void cord_common_init(void);

/**
 * @brief   Get the local endpoint identifier
 */
static inline const char *cord_common_get_ep(void)
{
    return (const char *)cord_common_ep;
}

/**
 * @brief   Add selected query string options to a gcoap request
 *
 * This function adds:
 *  - `ep` -> as extracted by cord_common_get_ep()
 *  - [optional] `lt` -> if defined by CONFIG_CORD_LT
 *  - [optional] 'd'  -> if defined by CORD_D
 *
 *  @return  0 on success
 *  @return  <0 on error
 */
int cord_common_add_qstring(coap_pkt_t *pkt);

#ifdef __cplusplus
}
#endif

/** @} */
