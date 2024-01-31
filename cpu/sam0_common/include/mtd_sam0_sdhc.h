/*
 * Copyright (C) 2022 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_sam0_sdhc MTD wrapper for SAM0 SDHC devices
 * @ingroup     drivers_storage
 * @ingroup     cpu_sam0_common_sdhc
 * @brief       Driver for SD Cards connected to the SAM0 SDHC peripheral using the MTD interface
 *
 * @{
 *
 * @file
 * @brief       Interface definition for SAM SDHC driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef MTD_SAM0_SDHC_H
#define MTD_SAM0_SDHC_H

#include <stdint.h>

#include "mtd.h"
#include "sdhc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd_sdcard device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                    /**< inherit from mtd_dev_t object */
    sdhc_state_t state;                /**< sdcard_spi dev descriptor */
} mtd_sam0_sdhc_t;

/**
 * @brief   sdcard device operations table for mtd
 */
extern const mtd_desc_t mtd_sam0_sdhc_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_SAM0_SDHC_H */
/** @} */
