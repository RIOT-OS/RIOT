/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca95xx
 * @{
 *
 * @file
 * @brief       Register definition for PCA95xx devices
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef PCA95XX_REGS_H
#define PCA95XX_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    PCA95xx register addesses
 *
 * Register addresses are multiplied by 2 on 16-bit devices
 * @{
 */
#define PCA95XX_INPUT_ADDR     (0)
#define PCA95XX_OUTPUT_ADDR    (1)
#define PCA95XX_POLAR_ADDR     (2)
#define PCA95XX_CONFIG_ADDR    (3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PCA95XX_REGS_H */
/** @} */
