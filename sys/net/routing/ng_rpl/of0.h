/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ng_rpl
 * @{
 * @file
 * @brief       Objective Function Zero.
 *
 * Header-file, which defines all functions for the implementation of Objective Function Zero.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 */

#ifndef OF0_H
#define OF0_H

#include "net/ng_rpl/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return the address to the of0 objective function
 *
 * @return  Address of the of0 objective function
 */
ng_rpl_of_t *ng_rpl_get_of0(void);

#ifdef __cplusplus
}
#endif

#endif /* OF0_H */
/**
 * @}
 */
