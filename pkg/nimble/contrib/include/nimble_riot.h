/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_contrib RIOT Integration
 * @ingroup     pkg_nimble
 * @brief       Basic RIOT integration of NimBLE, including e.g. stack
 *              configuration and (auto)initialization code
 * @{
 *
 * @file
 * @brief       RIOT specific glue functions for integrating NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_RIOT_H
#define NIMBLE_RIOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Priority used for NimBLE's controller thread
 *
 * This should be as high as possible.
 */
#ifndef NIMBLE_CONTROLLER_PRIO
#define NIMBLE_CONTROLLER_PRIO      (0)
#endif

/**
 * @brief   Stacksize used for NimBLE's controller thread
 */
#ifndef NIMBLE_CONTROLLER_STACKSIZE
#define NIMBLE_CONTROLLER_STACKSIZE (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Priority used for NimBLE's host thread
 */
#ifndef NIMBLE_HOST_PRIO
#define NIMBLE_HOST_PRIO            (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Stacksize used for NimBLE's host thread
 */
#ifndef NIMBLE_HOST_STACKSIZE
#define NIMBLE_HOST_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Export our own address type for later usage
 */
extern uint8_t nimble_riot_own_addr_type;

/**
 * @brief   Setup and run NimBLE's controller and host threads
 */
void nimble_riot_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_RIOT_H */
/** @} */
