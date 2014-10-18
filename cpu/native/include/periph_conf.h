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

#ifdef MODULE_NATIVE_GPIO_VIRTUAL       /* virtual */
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
#define GPIO_16_EN  (1)
#define GPIO_17_EN  (1)
#define GPIO_18_EN  (1)
#define GPIO_19_EN  (1)
#define GPIO_20_EN  (1)
#define GPIO_21_EN  (1)
#define GPIO_22_EN  (1)
#define GPIO_23_EN  (1)
#define GPIO_24_EN  (1)
#define GPIO_25_EN  (1)
#define GPIO_26_EN  (1)
#define GPIO_27_EN  (1)
#define GPIO_28_EN  (1)
#define GPIO_29_EN  (1)
#define GPIO_30_EN  (1)
#define GPIO_31_EN  (1)
#define GPIO_NUMOF (32)

#elif defined MODULE_NATIVE_GPIO_SYSFS  /* sysfs */

#ifdef NATIVE_GPIO_NUMOF
#   if NATIVE_GPIO_NUMOF > 32
#       define GPIO_NUMOF 32
#   else
#       define GPIO_NUMOF NATIVE_GPIO_NUMOF
#   endif
#else
#    define GPIO_NUMOF 0
#endif

#if GPIO_NUMOF > 0
#   define GPIO_0_EN    (1)
#endif
#if GPIO_NUMOF > 1
#   define GPIO_1_EN    (1)
#endif
#if GPIO_NUMOF > 2
#   define GPIO_2_EN    (1)
#endif
#if GPIO_NUMOF > 3
#   define GPIO_3_EN    (1)
#endif
#if GPIO_NUMOF > 4
#   define GPIO_4_EN    (1)
#endif
#if GPIO_NUMOF > 5
#   define GPIO_5_EN    (1)
#endif
#if GPIO_NUMOF > 6
#   define GPIO_6_EN    (1)
#endif
#if GPIO_NUMOF > 7
#   define GPIO_7_EN    (1)
#endif
#if GPIO_NUMOF > 8
#   define GPIO_8_EN    (1)
#endif
#if GPIO_NUMOF > 9
#   define GPIO_9_EN    (1)
#endif
#if GPIO_NUMOF > 10
#   define GPIO_10_EN   (1)
#endif
#if GPIO_NUMOF > 11
#   define GPIO_11_EN   (1)
#endif
#if GPIO_NUMOF > 12
#   define GPIO_12_EN   (1)
#endif
#if GPIO_NUMOF > 13
#   define GPIO_13_EN   (1)
#endif
#if GPIO_NUMOF > 14
#   define GPIO_14_EN   (1)
#endif
#if GPIO_NUMOF > 15
#   define GPIO_15_EN   (1)
#endif

#else                                   /* no GPIO device */
#define GPIO_NUMOF (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
