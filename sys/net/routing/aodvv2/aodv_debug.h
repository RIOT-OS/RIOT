/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @brief       Debug-header for aodvv2 debug messages
 *
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODV_DEBUG_H_
#define AODV_DEBUG_H_

#include <stdio.h>
#include "sched.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if ENABLE_DEBUG
#define ENABLE_AODV_DEBUG (1)
#endif

/**
 * @brief Print aodvv2 specific debug information to std-out with [aodvv2] prefix
 *
 */
#if ENABLE_AODV_DEBUG
#include "tcb.h"
#define AODV_DEBUG(...) \
 do { \
        printf("[aodvv2] "); \
        printf(__VA_ARGS__); \
    } while (0)
#else
#define AODV_DEBUG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* AODVV2_DEBUG_H_*/
/** @} */
