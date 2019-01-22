/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml1x
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[1][32] = {
    { 0,  1,  2,  3,  4,  5,  6,  7, -1, 0,  1,  2, -1, -1, 3, 4,
      5,  6,  7,  0, -1, -1,  1,  2,  3, 4, -1,  5, -1, -1, 6, 7},
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
