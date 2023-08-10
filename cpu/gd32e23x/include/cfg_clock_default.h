/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       Default clock configuration for GD32E23x
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 *
 */

#ifndef CFG_CLOCK_DEFAULT_H
#define CFG_CLOCK_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GD32E23x clock settings
 * @{
 */

#ifndef CONFIG_CLOCK_PLL_PREDIV
#define CONFIG_CLOCK_PLL_PREDIV         (1)
#endif

#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL            (12)
#endif

#define CLOCK_AHB                       CLOCK_CORECLOCK

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)

#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_CLOCK_DEFAULT_H */
/** @} */
