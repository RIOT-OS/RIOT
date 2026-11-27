/*
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     net_gnrc_rpl
 * @{
 * @file
 * @brief       Minimum Rank with Hysteresis Objective Function
 *
 * Header-file, which defines all functions for the implementation of Minimum Rank
 * with Hysteresis Objective Function.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "net/gnrc/rpl/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimal improvement over old route needed to switch parent
 */
#define MRHOF_PARENT_SWITCH_THRESHOLD   192

/**
 * @brief   Highest per-hop cost
 */
#define MRHOF_MAX_LINK_METRIC           512

/**
 * @brief   Highest path cost
 */
#define MRHOF_MAX_PATH_COST           3276

/**
 * @brief   Return the address to the MRH objective function
 *
 * @return  Address of the MRH objective function
 */
gnrc_rpl_of_t *gnrc_rpl_get_of_mrhof(void);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
