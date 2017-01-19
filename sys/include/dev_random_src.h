/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  sys_dev_random
 * @{
 *
 * @file
 * @brief   Randomness source definitions for usage in the `board.h`.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef DEV_RANDOM_SRC_H_
#define DEV_RANDOM_SRC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Randomness sources
 * @brief   Randomness sources for usage in the `board.h`
 *
 * A board can use these values to define its randomness source with the
 * @ref DEV_RANDOM_SRC macro.
 * @{
 */
/**
 * @brief   Use @ref drivers_periph_hwrng as randomness source
 */
#define DEV_RANDOM_SRC_PERIPH_HWRNG (1)

/**
 * @brief   Use @ref drivers_periph_adc as source
 *
 * The `board.h` is required to define the `DEV_RANDOM_ADC` macro with
 * the `ADC_LINE(x)` macro of the desired ADC.
 */
#define DEV_RANDOM_SRC_PERIPH_ADC   (2)

/**
 * @brief   Use randomness source of @ref drivers_at86rf2xx as source
 *
 * The global `dev_random_at86rf2xx` variable is required to be set after the
 * device was initialized.
 */
#define DEV_RANDOM_SRC_AT86RF2XX    (3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DEV_RANDOM_SRC_H_ */
/** @} */
