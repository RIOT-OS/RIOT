/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 *
 * @file
 * @brief       RIOT scheduler types variable declaration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "opendefs.h"
#include "scheduler.h"
#include "event/callback.h"
#include "memarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   OpenWSN scheduler variables structure
 */
typedef struct {
    event_callback_t task_buff[TASK_LIST_DEPTH]; /**< Task buffer */
    memarray_t memarray; /**< Memarray management */
} scheduler_vars_t;

#if SCHEDULER_DEBUG_ENABLE
/**
 * @brief   OpenWSN scheduler debug variables
 */
typedef struct {
    uint8_t numTasksCur;    /**< Current task number */
    uint8_t numTasksMax;    /**< Current peak queued tasks */
} scheduler_dbg_t;
#endif

#ifdef __cplusplus
}
#endif
