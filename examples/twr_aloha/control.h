/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "timex.h"

/**
 * @brief Anchor address
 */
#ifndef ANCHOR_ADDRESS
#define ANCHOR_ADDRESS      0x1234
#endif

/**
 * @brief Range request period
 */
#ifndef RANGE_REQUEST_T_MS
#define RANGE_REQUEST_T_MS      (40)
#endif

/**
 * @brief Starts ranging
 */
void init_ranging(void);

#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */
