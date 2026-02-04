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
 * @brief       RIOT mock scheduler types variable declaration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "scheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   OpenWSN scheduler variables structure
 */
typedef struct {
    uint8_t last_task;
} scheduler_vars_t;

#ifdef __cplusplus
}
#endif
