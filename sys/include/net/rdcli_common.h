/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_common CoRE RD Client Common
 * @ingroup     net
 * @brief       Shared functionality for CoRE Resource Directory clients
 * @{
 *
 * @file
 * @brief       Shared CoRE RD client functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_RDCLI_COMMON_H
#define NET_RDCLI_COMMON_H

#include "net/rdcli_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Export the local endpoint identifier
 *
 * @note    Use rdcli_common_get_ep() for accessing the endpoint identifier
 */
extern char rdcli_ep[];

/**
 * @brief   Generate unique endpoint identifier (ep)
 */
void rdcli_common_init(void);

/**
 * @brief   Get the local endpoint identifier
 */
static inline const char *rdcli_common_get_ep(void)
{
    return (const char *)rdcli_ep;
}

/**
 * @brief   Add selected query string options to a gcoap request
 *
 * This function adds:
 *  - `ep` -> as extracted by rdcli_commont_get_ep()
 *  - [optional] `lt` -> if defined by RDCLI_LT
 *  - [optional] 'd'  -> if defined by RDCLI_D
 *
 *  @return  0 on success
 *  @return  <0 on error
 */
int rdcli_common_add_qstring(coap_pkt_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_COMMON_H */
/** @} */
