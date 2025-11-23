/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       Glue code linking RPL with the NimBLE rpble connection manager
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

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

/** @} */
