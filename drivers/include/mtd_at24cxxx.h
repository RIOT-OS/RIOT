/*
 * Copyright (C) 2020 Otto-von-Guericke Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MTD_AT24CXXX_H
#define MTD_AT24CXXX_H

/**
 * @defgroup    drivers_mtd_at24cxxx MTD wrapper for AT24cxxx family of I2C-EEPROMs
 * @ingroup     drivers_storage
 * @brief       MTD wrapper for AT24XXX I2C based EEPROMs
 *
 * @{
 *
 * @file
 * @brief       Interface definition for at24cxxx MTD wrapper
 *
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "at24cxxx.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd at24cxxx device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                     /**< inherit from mtd_dev_t object */
    at24cxxx_t *at24cxxx_eeprom;        /**< at24cxxx device descriptor */
    const at24cxxx_params_t* params;    /**< at24cxxx parameters */
} mtd_at24cxxx_t;

/**
 * @brief Shortcut initializer for @ref mtd_at24cxxx_t
 */
#define MTD_AT24CXXX_INIT(dev_p, params_p)          \
(mtd_at24cxxx_t) {                                  \
    .base = {                                       \
        .driver = &mtd_at24cxxx_driver              \
    },                                              \
    .at24cxxx_eeprom = (dev_p),                     \
    .params = (params_p)                            \
}

/**
 * @brief MTD EEPROM driver for at24cxxx EEPROM
 */
extern const mtd_desc_t mtd_at24cxxx_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MTD_AT24CXXX_H */
