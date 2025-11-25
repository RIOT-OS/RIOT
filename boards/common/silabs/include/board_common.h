/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

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

#endif /* BOARD_COMMON_H */
/** @} */
