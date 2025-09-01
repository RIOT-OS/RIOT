/*
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_native
 *
 * The native boards use call level hardware simulation
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the native boards
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include <stdint.h>

/* RIOT includes */
#include <motor_driver.h>

#ifdef __cplusplus
extern "C" {
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
#if defined(MODULE_FATFS) || defined(MODULE_LWEXT4)
#define MTD_PAGE_SIZE           (512)
#else
#define MTD_PAGE_SIZE           (256)
#endif
#endif
#ifndef MTD_SECTOR_SIZE
#if defined(MODULE_FATFS) || defined(MODULE_LWEXT4)
#define MTD_SECTOR_SIZE         (512)
#else
#define MTD_SECTOR_SIZE         (4096)
#endif
#endif
#ifndef MTD_SECTOR_NUM
#if defined(MODULE_FATFS) || defined(MODULE_LWEXT4)
#define MTD_SECTOR_NUM          (32768)
#else
#define MTD_SECTOR_NUM          (2048)
#endif
#endif
/** Advertised write size. While the file system backend supports single byte
 * granularity, this can be increased to mimic other media. */
#ifndef MTD_WRITE_SIZE
#define MTD_WRITE_SIZE          (1)
#endif
#ifndef MTD_NATIVE_FILENAME
#define MTD_NATIVE_FILENAME     "MEMORY.bin"
#endif
/** @} */

/** Default MTD device (mtd flash emulation device) */
#define MTD_0 mtd_dev_get(0)
#endif

/**
 * @name    Host FS access configuration
 * @{
 */
#ifndef FS_NATIVE_DIR
#define FS_NATIVE_DIR           "native"  /**< Folder on the host fs exported to RIOT */
#endif
/** @} */

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

#if MODULE_PERIPH_QDEC
/**
 * @brief Simulate QDEC on motor_set() calls
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 * @param[in] pwm_duty_cycle    Signed PWM duty_cycle to set motor speed and direction
 */
void native_motor_driver_qdec_simulation( \
    const motor_driver_t motor_driver, uint8_t motor_id, \
    int32_t pwm_duty_cycle);

/* C++ standard do not support designated initializers */
#if !(defined __cplusplus) && (defined MODULE_PERIPH_QDEC)

/**
 * @name Describe DC motors with PWM channel and GPIOs
 * @{
 */
static const motor_driver_config_t motor_driver_config[] = {
    {
        .pwm_dev         = 0,
        .mode            = MOTOR_DRIVER_1_DIR_BRAKE,
        .mode_brake      = MOTOR_BRAKE_LOW,
        .pwm_mode        = PWM_LEFT,
        .pwm_frequency   = 20000U,
        .pwm_resolution  = 1000U,
        .nb_motors       = 2,
        .motors          = {
            {
                .pwm_channel            = 0,
                .gpio_enable            = GPIO_PIN(0, 0),
                .gpio_dir0              = GPIO_PIN(0, 0),
                .gpio_dir1_or_brake     = GPIO_PIN(0, 0),
                .gpio_dir_reverse       = 0,
                .gpio_enable_invert     = 0,
                .gpio_brake_invert      = 0,
            },
            {
                .pwm_channel            = 1,
                .gpio_enable            = GPIO_PIN(0, 0),
                .gpio_dir0              = GPIO_PIN(0, 0),
                .gpio_dir1_or_brake     = GPIO_PIN(0, 0),
                .gpio_dir_reverse       = 1,
                .gpio_enable_invert     = 0,
                .gpio_brake_invert      = 0,
            },
        },
        .cb = native_motor_driver_qdec_simulation,
    },
};

#define MOTOR_DRIVER_NUMOF           ARRAY_SIZE(motor_driver_config)
/** @} */
#endif

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE    ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV     TIMER_DEV(0)
/* on native, anything can happen... */
#define CONFIG_ZTIMER_USEC_MIN     (64)
/** @} */

#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

/** @} */
