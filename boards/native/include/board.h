/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_native Native Board
 * @ingroup     boards
 * @brief       Board emulation for the native port
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

#ifdef MODULE_MTD
#define MTD_0 mtd0

/** mtd flash emulation device */
extern mtd_dev_t *mtd0;
#endif

#ifdef MODULE_SPIFFS
#define SPIFFS_READ_ONLY 0
#define SPIFFS_SINGLETON 0

#define SPIFFS_HAL_CALLBACK_EXTRA 1

#define SPIFFS_CACHE 1

#if SPIFFS_SINGLETON == 1
#define SPIFFS_CFG_PHYS_SZ(ignore)        (0x800000)

#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (4096)

#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)

#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)

#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (4096)
#endif

#if SPIFFS_HAL_CALLBACK_EXTRA == 0
#define SPIFFS_MTD_DEV (MTD_0)
#endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
