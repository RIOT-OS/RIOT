/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_native
 *
 * The native board uses call level hardware simulation
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the native board
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_MTD
#include "mtd_native.h"
#endif

/**
 * @name    LED handlers
 * @{
 */
void _native_LED_GREEN_OFF(void);
void _native_LED_GREEN_ON(void);
void _native_LED_GREEN_TOGGLE(void);
void _native_LED_RED_OFF(void);
void _native_LED_RED_ON(void);
void _native_LED_RED_TOGGLE(void);

#define LED0_ON             (_native_LED_RED_ON())
#define LED0_OFF            (_native_LED_RED_OFF())
#define LED0_TOGGLE         (_native_LED_RED_TOGGLE())
#define LED1_ON             (_native_LED_GREEN_ON())
#define LED1_OFF            (_native_LED_GREEN_OFF())
#define LED1_TOGGLE         (_native_LED_GREEN_TOGGLE())
/** @} */

#if defined(MODULE_MTD) || DOXYGEN
/**
 * @name    MTD emulation configuration
 * @{
 */
#ifndef MTD_PAGE_SIZE
#define MTD_PAGE_SIZE           (256)
#endif
#ifndef MTD_SECTOR_SIZE
#define MTD_SECTOR_SIZE         (4096)
#endif
#ifndef MTD_SECTOR_NUM
#define MTD_SECTOR_NUM          (2048)
#endif
#ifndef MTD_NATIVE_FILENAME
#define MTD_NATIVE_FILENAME     "MEMORY.bin"
#endif
/** @} */

/** Default MTD device */
#define MTD_0 mtd0

/** mtd flash emulation device */
extern mtd_dev_t *mtd0;
#endif

#if defined(MODULE_SPIFFS) || DOXYGEN
/**
 * @name    SPIFFS default configuration
 * @{
 */
/* SPIFFS config flags */
#ifndef SPIFFS_READ_ONLY
#define SPIFFS_READ_ONLY                    (0)
#endif
#ifndef SPIFFS_SINGLETON
#define SPIFFS_SINGLETON                    (0)
#endif
#ifndef SPIFFS_HAL_CALLBACK_EXTRA
#define SPIFFS_HAL_CALLBACK_EXTRA           (1)
#endif
#ifndef SPIFFS_CACHE
#define SPIFFS_CACHE                        (1)
#endif

#if SPIFFS_SINGLETON == 1
/* MTD config if singleton is used */
#ifndef SPIFFS_CFG_PHYS_SZ
#define SPIFFS_CFG_PHYS_SZ(ignore)          (MTD_SECTOR_SIZE * MTD_SECTOR_NUM)
#endif
#ifndef SPIFFS_CFG_PHYS_ERASE_SZ
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)    (MTD_SECTOR_SIZE)
#endif
#ifndef SPIFFS_CFG_PHYS_ADDR
#define SPIFFS_CFG_PHYS_ADDR(ignore)        (0)
#endif
#ifndef SPIFFS_CFG_LOG_PAGE_SZ
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)      (MTD_PAGE_SIZE)
#endif
#ifndef SPIFFS_CFG_LOG_BLOCK_SZ
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)     (MTD_SECTOR_SIZE)
#endif
#endif

#if SPIFFS_HAL_CALLBACK_EXTRA == 0
/* Default MTD device if no callback parameter */
#ifndef SPIFFS_MTD_DEV
#define SPIFFS_MTD_DEV                      (MTD_0)
#endif
#endif
/** @} */
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
