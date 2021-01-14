/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     drivers_cst816s
 *
 * @{
 * @file
 * @brief       Internal constants for cst816s
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */
#ifndef CST816S_INTERNAL_H
#define CST816S_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    cst816s timing constants
 * @{
 */
#define CST816S_RESET_DURATION_LOW      (20 * US_PER_MS)
#define CST816S_RESET_DURATION_HIGH     (400 * US_PER_MS)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CST816S_INTERNAL_H */
/** @} */
