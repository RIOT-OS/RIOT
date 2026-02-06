/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name GP8xxx registers
 * @{
 */
#define GP8XXX_REG_OUTPUT_RANGE     (0x01)  /**< Register for output range */
#define GP8XXX_REG_CHANNEL          (0x02)  /**< Register for channel selection */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
