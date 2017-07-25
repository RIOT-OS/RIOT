/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_common Shared Functions for CoRE RD Clients
 * @ingroup     net_rdcli
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

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_COMMON_H */
/** @} */
