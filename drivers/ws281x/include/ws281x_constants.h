/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 * @file
 * @brief       Constants for WS2812/SK6812 RGB LEDs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Data encoding parameters for WS2812/SK6812 RGB LEDs
 * @{
 */
/**
 * @brief   Offset for the red color component
 */
#define WS281X_OFFSET_R                 (1U)
/**
 * @brief   Offset for the green color component
 */
#define WS281X_OFFSET_G                 (0U)
/**
 * @brief   Offset for the blue color component
 */
#define WS281X_OFFSET_B                 (2U)
/**
 * @brief   Offset for the white color component (only for 4-byte variants)
 */
#define WS281X_OFFSET_W                 (3U)
/**@}*/

#ifdef __cplusplus
}
#endif

/** @} */
