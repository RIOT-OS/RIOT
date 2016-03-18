/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml21
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the SAML21 for convenient access
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
};

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: PU or PU
 * - bit 1: input enable
 * - bit 2: pull enable
 */
#define GPIO_MODE(pr, ie, pe)   (pr | (ie << 1) | (pe << 2))

/**
 * @brief   Override GPIO modes
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 1, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(1, 1, 1),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(0, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = 0xfe,                  /**< not supported by HW */
    GPIO_OD_PU = 0xff                   /**< not supported by HW */
} gpio_mode_t;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
