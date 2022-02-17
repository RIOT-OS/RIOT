/*
 * Copyright (C) 2020 Otto-von-Guericke Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_at24xxxx MTD wrapper for AT24XXXX family of I2C-EEPROMs
 * @ingroup     drivers_storage
 * @brief       MTD wrapper for AT24XXX I2C based EEPROMs
 *
 * @{
 *
 * @file
 * @brief       Interface definition for at24xxxx MTD wrapper
 *
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef MTD_AT24XXXX_H
#define MTD_AT24XXXX_H

#include "at24xxxx.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd at24xxxx device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                     /**< inherit from mtd_dev_t object */
    at24xxxx_t *at24xxxx_eeprom;        /**< at24xxxx device descriptor */
    const at24xxxx_params_t* params;    /**< at24xxxx parameters */
} mtd_at24xxxx_t;

/**
 * @brief Shortcut initializer for @ref mtd_at24xxxx_t
 */
#define MTD_AT24XXXX_INIT(dev_p, params_p)          \
(mtd_at24xxxx_t) {                                  \
    .base = {                                       \
        .driver = &mtd_at24xxxx_driver              \
    },                                              \
    .at24xxxx_eeprom = (dev_p),                     \
    .params = (params_p)                            \
}

/**
 * @brief MTD EEPROM driver for at24xxxx EEPROM
 */
extern const mtd_desc_t mtd_at24xxxx_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_AT24XXXX_H */
/** @} */
