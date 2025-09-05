/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_cst816s
 *
 * @{
 * @file
 * @brief       Internal constants for cst816s
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    cst816s timing constants
 * @{
 */
#define CST816S_RESET_DURATION_LOW_MS       (20)    /**< Duration of reset pin low (in ms) */
#define CST816S_RESET_DURATION_HIGH_MS      (400)   /**< Duration of reset pin high (in ms) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
