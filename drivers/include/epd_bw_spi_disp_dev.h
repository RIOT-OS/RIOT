/*
 * Copyright (C) 2020 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_epd_bw_spi
 * @{
 *
 * @file
 * @brief       Definition of the driver for the disp_dev generic interface
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#include "disp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the display device driver struct
 */
extern const disp_dev_driver_t epd_bw_spi_disp_dev_driver;

#ifdef __cplusplus
}
#endif

/** @} */
