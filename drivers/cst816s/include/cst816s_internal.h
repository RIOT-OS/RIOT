/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
