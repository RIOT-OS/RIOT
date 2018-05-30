/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wsn430-v1_4 WSN430 v1.4
 * @ingroup     boards
 * @brief       Support for the Senslab WSN430 v1.4 board
 *
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Senslab WSN430 v1.4 board
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
 * @name    Define the interface to the CC2420 radio
 * @{
 */
#define CC2420_PARAMS_PIN_CS            (GPIO_PIN(P4, 2))
#define CC2420_PARAMS_PIN_FIFO          (GPIO_PIN(P1, 3))
#define CC2420_PARAMS_PIN_FIFOP         (GPIO_PIN(P1, 4))
#define CC2420_PARAMS_PIN_CCA           (GPIO_PIN(P1, 6))
#define CC2420_PARAMS_PIN_SFD           (GPIO_PIN(P1, 5))
#define CC2420_PARAMS_PIN_VREFEN        (GPIO_PIN(P3, 0))
#define CC2420_PARAMS_PIN_RESET         (GPIO_PIN(P1, 7))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
