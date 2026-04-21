/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lcd
 * @{
 *
 * @file
 * @brief       Definition of the driver for the disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "disp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the display device driver struct
 */
extern const disp_dev_driver_t lcd_disp_dev_driver;

#ifdef __cplusplus
}
#endif

/** @} */
