/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f334 Nucleo-F334
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f334 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f334 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer tuning values
 * @{
 */
#define XTIMER_OVERHEAD     5
#define XTIMER_SHOOT_EARLY  2
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
