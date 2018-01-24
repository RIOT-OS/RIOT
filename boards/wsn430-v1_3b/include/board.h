/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wsn430-v1_3b WSN430 v1.3b
 * @ingroup     boards
 * @brief       Support for the Senslab WSN430 v1.3b board
 *
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Senslab WSN430 v1.3b board
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Define the interface to the CC1101 radio
 * @{
 */
#define CC110X_PARAM_CS             (GPIO_PIN(P4, 2))
#define CC110X_PARAM_GDO0           (GPIO_PIN(P1, 3))
#define CC110X_PARAM_GDO1           (GPIO_PIN(P5, 2))
#define CC110X_PARAM_GDO2           (GPIO_PIN(P1, 4))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
