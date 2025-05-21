/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_rpl
 * @{
 * @file
 * @brief       Objective Function Zero.
 *
 * Header-file, which defines all functions for the implementation of Objective Function Zero.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 */

#include "net/gnrc/rpl/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return the address to the of0 objective function
 *
 * @return  Address of the of0 objective function
 */
gnrc_rpl_of_t *gnrc_rpl_get_of0(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
