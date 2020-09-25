/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Default configuration for the OpenWSN debugpins
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef OPENWSN_DEBUGPINS_PARAMS_H
#define OPENWSN_DEBUGPINS_PARAMS_H

#include "board.h"
#include "openwsn_debugpins.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    OpenWSN default debugpins configuration.
 *
 * Undefined by default.
 * @{
 */
#ifndef OPENWSN_DEBUGPIN_FRAME
#define OPENWSN_DEBUGPIN_FRAME          GPIO_UNDEF
#endif
#ifndef OPENWSN_DEBUGPIN_SLOT
#define OPENWSN_DEBUGPIN_SLOT           GPIO_UNDEF
#endif
#ifndef OPENWSN_DEBUGPIN_FSM
#define OPENWSN_DEBUGPIN_FSM            GPIO_UNDEF
#endif
#ifndef OPENWSN_DEBUGPIN_TASK
#define OPENWSN_DEBUGPIN_TASK           GPIO_UNDEF
#endif
#ifndef OPENWSN_DEBUGPIN_ISR
#define OPENWSN_DEBUGPIN_ISR            GPIO_UNDEF
#endif
#ifndef OPENWSN_DEBUGPIN_RADIO
#define OPENWSN_DEBUGPIN_RADIO          GPIO_UNDEF
#endif

#define OPENWSN_DEBUGPINS_DEFAULT    { .frame = OPENWSN_DEBUGPIN_FRAME, \
                                       .slot = OPENWSN_DEBUGPIN_SLOT, \
                                       .fsm = OPENWSN_DEBUGPIN_FSM,  \
                                       .task = OPENWSN_DEBUGPIN_TASK, \
                                       .isr = OPENWSN_DEBUGPIN_ISR,  \
                                       .radio = OPENWSN_DEBUGPIN_RADIO}
/**@}*/

/**
 * @brief   OpenWSN debugpins configuration
 */
static const debugpins_config_t openwsn_debugpins_params[] =
{
#ifdef OPENWSN_DEBUGPINS_BOARD
    OPENWSN_DEBUGPINS_BOARD,
#else
    OPENWSN_DEBUGPINS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_DEBUGPINS_PARAMS_H */
