/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_silabs_drivers_bc Board communication controller
 * @ingroup     boards_common_silabs_drivers
 * @{
 *
 * @file
 * @brief       Implementations of the board controller driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef BC_H
#define BC_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the board controller.
 *
 * The following values must be defined:
 *
 * - BC_PIN
 */
void bc_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BC_H */
