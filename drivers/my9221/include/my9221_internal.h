/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_my9221
 *
 * @{
 * @file
 * @brief       Internal config and parameters for the MY9221 LED controller
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 	Time to wait until latch register is processed
 */
#define MY9221_LATCH_WAIT       (10U * US_PER_MS)

/**
 * @brief 	Number of write loops for latch register
 */
#define MY9221_LATCH_LOOP       (4U)

/**
 * @brief 	Enable command mode on LED controller
 */
#define MY9221_CMDMODE          (0x00)

#ifdef __cplusplus
}
#endif

/** @} */
