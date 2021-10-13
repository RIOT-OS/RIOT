/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 * @{
 *
 * @file
 * @brief       Definition of the driver for the touch_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_TOUCH_DEV_H
#define STMPE811_TOUCH_DEV_H

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

#endif /* STMPE811_TOUCH_DEV_H */
