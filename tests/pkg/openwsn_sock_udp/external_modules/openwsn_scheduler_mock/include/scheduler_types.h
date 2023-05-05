/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef SCHEDULER_TYPES_H
#define SCHEDULER_TYPES_H

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

#endif /* SCHEDULER_TYPES_H */
