/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 * @author      Imagination Technologies.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "p32mx470f512h.h"

/**
 * @brief The peripheral clock is required for the UART Baud rate calculation
 *        It is configured by the 'config' registers (see pic32_config_settings.c)
 *        Note 120MHz is the max F for this device.
 */
#define PERIPHERAL_CLOCK (120000000)  /* Hz */

/**
 * @brief Set how many increments of the count register per uS
 *        needed by the timer code.
 */
#define TICKS_PER_US (60)

/**
 * @brief We are using a PIC32MX device
 */
#define PIC32MX (1)

#ifdef __cplusplus
}
#endif


#endif /* _BOARD_H_ */
/** @} */

