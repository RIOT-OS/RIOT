/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nrf52 NRF52 common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for all nRF52 boards.
 *
 * # Flashing
 * ## Using JLink
 * By default the nRF52 family of boards is flashed using the Segger's JLink
 * tool. Due to its proprietary nature, only pre-build binary blobs are
 * available for only the most common systems. You can use OpenOCD on those
 * systems instead. If the JLink tool is installed on your system, you can
 * compile and flash your application by running:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * make BOARD=<nRF52_BOARD> flash
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Using OpenOCD
 * As of January 2018, the latest stable version of OpenOCD (version 0.10.0) is
 * not able to flash nRF52 boards. Thus, you will currently need to build the
 * current development version of OpenOCD in order to flash your boards using
 * OpenOCD. If the development version of OpenOCD is installed, you can build
 * and flash your application by running:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * make BOARD=<nRF52_BOARD> PROGRAMMER=openocd flash
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Shared configuration for the all nRF52-based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the platform
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
