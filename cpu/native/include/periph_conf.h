/**
 * Native CPU peripheral configuration
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (1U)
/** @} */

/**
 * @name RealTime Clock configuration
 * @{
 */
#define RTC_NUMOF (1)
/** @} */

#ifdef MODULE_NATIVE_GPIO_DUMMY
#define GPIO_0_EN   (1)
#define GPIO_1_EN   (1)
#define GPIO_2_EN   (1)
#define GPIO_3_EN   (1)
#define GPIO_4_EN   (1)
#define GPIO_5_EN   (1)
#define GPIO_6_EN   (1)
#define GPIO_7_EN   (1)
#define GPIO_8_EN   (1)
#define GPIO_9_EN   (1)
#define GPIO_10_EN  (1)
#define GPIO_11_EN  (1)
#define GPIO_12_EN  (1)
#define GPIO_13_EN  (1)
#define GPIO_14_EN  (1)
#define GPIO_15_EN  (1)
#define GPIO_NUMOF (16)

#else /* no GPIO device */
#define GPIO_NUMOF (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
