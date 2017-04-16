/*
 * Copyright(C) 2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    boards_pic32-wifire Digilent PIC32 WiFire
 * @ingroup     boards
 * @brief       board configuration for the Digilent PIC32 WiFire
 * @details
 * See:
 * http://store.digilentinc.com/chipkit-wi-fire-wifi-enabled-mz-microcontroller-board/
 * for more information on the board.
 *
 * @{
 *
 * @file
 * @brief       board configuration for the Digilent PIC32 WiFire
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vendor/p32mz2048efg100.h"

/**
 * @brief   Set how many increments of the count register per uS
 *          needed by the timer code.
 */
#define TICKS_PER_US (100)

/**
 * @brief   We are using an External Interrupt Controller (all pic32 devices use this mode)
 */
#define EIC_IRQ      (1)

/**
 * @brief   Board level initialisation
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H_ */
/** @} */
