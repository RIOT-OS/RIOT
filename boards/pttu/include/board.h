/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_pttu PTTU
 * @ingroup     boards
 * @brief       Support for the PTTU board
 * @{
 *
 * @file
 * @brief       Basic definitions for the PTTU board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "lpc2387.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   VICIntEnClear Alias for compatibility
 */
#define VICIntEnClear VICIntEnClr

/**
 * @brief   Clock initialization part one
 */
void init_clks1(void);

/**
 * @brief   Clock initialization part two
 */
void init_clks2(void);

/**
 * @brief   Jump to clock initialization code
 */
void bl_init_clks(void);

/**
 * @brief   Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
/** @} */
