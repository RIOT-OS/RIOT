/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   drivers_mtd_native Native MTD
 * @ingroup    drivers_mtd
 * @brief      MTD flash emulation for native
 * @{
 */

/**
 * @file
 * @author Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mtd.h"

/** mtd native descriptor */
typedef struct mtd_native_dev {
    mtd_dev_t base;     /**< mtd generic device */
    const char *fname;  /**< filename to use for memory emulation */
} mtd_native_dev_t;

/**
 * @brief Native mtd flash driver
 */
extern const mtd_desc_t native_flash_driver;

#ifdef __cplusplus
}
#endif

/** @} */
