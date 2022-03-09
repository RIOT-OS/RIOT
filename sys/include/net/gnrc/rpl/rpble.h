/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       Glue code linking RPL with the NimBLE rpble connection manager
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_GNRC_RPL_RPBLE_H
#define NET_GNRC_RPL_RPBLE_H

#include <string.h>
#if IS_USED(MODULE_NIMBLE_RPBLE)
#include "nimble_rpble.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_NIMBLE_RPBLE)
static inline void gnrc_rpl_rpble_update(const gnrc_rpl_dodag_t *dodag)
{
    nimble_rpble_ctx_t ctx;
    ctx.inst_id = dodag->instance->id;
    memcpy(ctx.dodag_id, &dodag->dodag_id, 16);
    ctx.version = dodag->version;
    ctx.rank = dodag->my_rank;
    ctx.role = dodag->node_status;
    nimble_rpble_update(&ctx);
}
#else
static inline void gnrc_rpl_rpble_update(const gnrc_rpl_dodag_t *dodag)
{
    (void)dodag;
    /* do nothing here */
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_RPL_RPBLE_H */
/** @} */
