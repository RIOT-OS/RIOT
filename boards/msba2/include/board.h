/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msba2 MSB-A2
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-A2 board
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-A2 board
 *
 * @author      unknown
 */

#ifndef BOARD_H
#define BOARD_H

#include "msba2_common.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_RED_PIN (BIT25)
#define LED_GREEN_PIN (BIT26)

#define LED_GREEN_OFF (FIO3SET = LED_GREEN_PIN)
#define LED_GREEN_ON (FIO3CLR = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE (FIO3PIN ^= LED_GREEN_PIN)

#define LED_RED_OFF (FIO3SET = LED_RED_PIN)
#define LED_RED_ON (FIO3CLR = LED_RED_PIN)
#define LED_RED_TOGGLE (FIO3PIN ^= LED_RED_PIN)

void init_clks1(void);

typedef uint8_t radio_packet_length_t;

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
