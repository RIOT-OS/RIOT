/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CONGURE_CONFIG_H
#define CONGURE_CONFIG_H
/**
 * @defgroup    sys_congure_config  CongURE compile time configuration
 * @ingroup     sys_congure
 * @ingroup     config
 * @brief   Configuration for congestion control using @ref sys_congure
 * @{
 *
 * @file
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default numerator of the ABE multiplier (0.8)
 *
 * @note Use with @ref sys_congure_abe
 * @see [RFC 8511, section 3.1](https://tools.ietf.org/html/rfc8511#section-3.1)
 */
#ifndef CONFIG_CONGURE_ABE_MULTIPLIER_NUMERATOR_DEFAULT
#define CONFIG_CONGURE_ABE_MULTIPLIER_NUMERATOR_DEFAULT     (4U)
#endif

/**
 * @brief   Default denominator of the ABE multiplier (0.8)
 *
 * @note Use with @ref sys_congure_abe
 * @see [RFC 8511, section 3.1](https://tools.ietf.org/html/rfc8511#section-3.1)
 */
#ifndef CONFIG_CONGURE_ABE_MULTIPLIER_DENOMINATOR_DEFAULT
#define CONFIG_CONGURE_ABE_MULTIPLIER_DENOMINATOR_DEFAULT   (5U)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CONGURE_CONFIG_H */
