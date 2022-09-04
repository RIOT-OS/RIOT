/*
 * Copyright (C) 2019 Inria
 * Copyright (c) 2022 Digitaluhr Manufaktur
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lpm013m126
 * @{
 *
 * @file
 * @brief       Definition of the driver for the disp_dev generic interface
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 */

#ifndef LPM013M126_DISP_DEV_H
#define LPM013M126_DISP_DEV_H

#include "disp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the display device driver struct
 */
extern const disp_dev_driver_t lpm013m126_disp_dev_driver;

#ifdef __cplusplus
}
#endif

#endif /* LPM013M126_DISP_DEV_H */
