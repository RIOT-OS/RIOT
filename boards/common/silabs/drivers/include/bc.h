/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
