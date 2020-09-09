/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Internal definitions for the ATWINC15x0 WiFi netdev driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ATWINC15X0_INTERNAL_H
#define ATWINC15X0_INTERNAL_H

#include "atwinc15x0.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the single ATWINC15x0 device instance
 *
 * Since the vendor ATWINC15x0 host driver uses many global variables, only
 * a single ATWINC15x0 device can be used. Therefore, the RIOT driver only
 * supports a single instance of an ATWINC15x0 device.
 */
extern atwinc15x0_t *atwinc15x0;

/**
 * @brief   ATWINC15x0 device driver ISR
 */
void atwinc15x0_irq(void);

#ifdef __cplusplus
}
#endif

#endif /* ATWINC15X0_INTERNAL_H */
/** @} */
