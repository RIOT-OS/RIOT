/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_at25xxx MTD wrapper for AT25xxx family of SPI-EEPROMs
 * @ingroup     drivers_storage
 * @brief       MTD wrapper for AT25XXX based SPI EEPROMs
 *
 * @{
 *
 * @file
 * @brief       Interface definition for at25xxx MTD wrapper
 *
 * @author      Johannes Koster <johannes.koster@ml-pa.com>
 */

#ifndef AT25XXX_MTD_H
#define AT25XXX_MTD_H

#include <stdint.h>

#include "at25xxx.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd_at25xxx device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                     /**< inherit from mtd_dev_t object */
    at25xxx_t *at25xxx_eeprom;          /**< at25xxx_eeprom dev descriptor */
    const at25xxx_params_t *params;     /**< params for at25xxx_eeprom init */
} mtd_at25xxx_t;

/**
 * @brief   mtd_at25xxx_eeprom device operations table for mtd
 */
extern const mtd_desc_t mtd_at25xxx_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AT25XXX_MTD_H */
