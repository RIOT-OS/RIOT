/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mx25v MX25V
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the MX25V
 * @{
 *
 * @file
 * @brief       MTD interface for the MX25V flash memory
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef MTD_MX25V_H
#define MTD_MX25V_H

#include "mtd.h"
#include "mx25v.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mtd_mx25v_dev {
    mtd_dev_t driver;
    mx25v_t dev;
} mtd_mx25v_dev_t;

#define MTD_MX25V_DESC(_dev, _spi, _spi_conf, _spi_speed, _cs_pin, _hold_pin, _wp_pin) \
    static mtd_mx25v_dev_t _dev = { \
        .driver = { \
            .driver = &mx25v_flash_interface, \
            .sector_count = MX25V_SECTOR_NUM, \
            .pages_per_sector = MX25V_SECTOR_SIZE / MX25V_PAGE_SIZE, \
            .page_size = MX25V_PAGE_SIZE, \
        }, \
        .dev = { \
            .spi_dev = _spi, \
            .conf = _spi_conf, \
            .speed = _spi_speed, \
            .cs_pin = _cs_pin, \
            .hold_pin = _hold_pin, \
            .wp_pin = _wp_pin, \
        }, \
    }

extern const mtd_desc_t mx25v_flash_interface;

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* MTD_MX25V_H */
