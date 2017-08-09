/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 * @file
 * @brief       Minimum Rank with Hysteresis Objective Function
 *
 * Header-file, which defines all functions for the implementation of Minimum Rank with Hysteresis Objective Function.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MRHOF_H
#define MRHOF_H

#include "net/gnrc/rpl/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MRHOF_PARENT_SWITCH_THRESHOLD   192
#define MRHOF_MAX_LINK_METRIC           512
#define MRHOF_MAX_PATH_COST           32768
#define MRHOF_PARENT_SET_SIZE             3

/**
 * @brief   Return the address to the of0 objective function
 *
 * @return  Address of the of0 objective function
 */
gnrc_rpl_of_t *gnrc_rpl_get_of_mrhof(void);

#ifdef __cplusplus
}
#endif

#endif /* MRHOF_H */
/**
 * @}
 */
