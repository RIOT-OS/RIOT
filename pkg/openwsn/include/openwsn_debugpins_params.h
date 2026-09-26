/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
