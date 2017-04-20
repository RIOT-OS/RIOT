/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    boards_pic32-clicker MikroE PIC32 Clicker
 * @ingroup     boards
 * @brief       board configuration for the MikroE PIC32 Clicker
 * @details
 * see:
 * http://www.mikroe.com/pic32/pic32mx-clicker/
 * For more information on the board.
 *
 * @{
 *
 * @file
 * @brief       board configuration for the MikroE PIC32 Clicker
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "vendor/p32mx470f512h.h"

/**
 * @brief   Set how many increments of the count register per uS
 *          needed by the timer code.
 */
#define TICKS_PER_US (48)

/**
 * @brief   We are using an External Interrupt Controller (all pic32 devices use this mode)
 */
#define EIC_IRQ (1)

/**
 * @brief   Board level initialisation
 */
void board_init(void);

#ifdef __cplusplus
}
#endif


#endif /* BOARD_H */
/** @} */
