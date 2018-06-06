/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble
 * @{
 *
 * @file
 * @brief       RIOT specific glue functions for integrating NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_RIOT_H
#define NIMBLE_RIOT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the priority used for NimBLE's controller thread
 *
 * This should be as high as possible.
 */
#ifndef NIMBLE_CONTROLLER_PRIO
#define NIMBLE_CONTROLLER_PRIO      (0)
#endif

/**
 * @brief   Starts a thread running NimBLE's controller
 */
void nimble_riot_controller_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_RIOT_H */
/** @} */
