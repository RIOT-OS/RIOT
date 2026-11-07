/*
 * SPDX-FileCopyrightText: 2020 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
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
