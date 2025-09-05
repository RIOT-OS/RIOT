/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_stmpe811
 * @{
 *
 * @file
 * @brief       Definition of the driver for the touch_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "touch_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the touch device driver struct
 */
extern const touch_dev_driver_t stmpe811_touch_dev_driver;

#ifdef __cplusplus
}
#endif

/** @} */
