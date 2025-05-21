/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup drivers_mtd
 * @defgroup   drivers_mtd_native Native MTD
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
