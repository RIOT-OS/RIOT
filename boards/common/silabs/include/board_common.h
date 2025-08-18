/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_silabs
 *
 * @{
 *
 * @file
 * @brief       Common board definitions for the Silicon Labs developtment
 *              boards.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize common board features.
 *
 * It currently initializes (if available) the AEM, BC, LEDs and PIC.
 */
void board_common_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
