/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp8266
 * @brief       Board definitions that are common for all ESP8266 boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

/* not required when compiling ESP vendor code parts */
#ifndef ESP_PLATFORM

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph_conf_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Common on-board LED control definitions
 * @{
 */
#ifdef  LED0_PIN
#define LED0_MASK   (BIT(LED0_PIN))
#define LED0_TOGGLE (gpio_toggle(LED0_PIN))
#define LED0_ON     (gpio_write(LED0_PIN, LED0_ACTIVE))
#define LED0_OFF    (gpio_write(LED0_PIN, !LED0_ACTIVE))
#endif

#ifdef  LED1_PIN
#define LED1_MASK   (BIT(LED1_PIN))
#define LED1_TOGGLE (gpio_toggle(LED1_PIN))
#define LED1_ON     (gpio_write(LED1_PIN, LED1_ACTIVE))
#define LED1_OFF    (gpio_write(LED1_PIN, !LED1_ACTIVE))
#endif

#ifdef  LED2_PIN
#define LED2_MASK   (BIT(LED2_PIN))
#define LED2_TOGGLE (gpio_toggle(LED2_PIN))
#define LED2_ON     (gpio_write(LED2_PIN, LED2_ACTIVE))
#define LED2_OFF    (gpio_write(LED2_PIN, !LED2_ACTIVE))
#endif
/** @} */

/**
 * @name    STDIO configuration
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (115200)    /**< Default baudrate of UART for stdio */
#endif
/** @} */

#ifndef DOXYGEN
/**
 * @name    XTimer configuration
 * @{
 */
#if defined(MODULE_ESP_SW_TIMER)
#define XTIMER_BACKOFF              (100U)
#define XTIMER_ISR_BACKOFF          (100U)
#endif /* MODULE_ESP_SW_TIMER */

/** @} */
#endif /* DOXYGEN */

#if defined(MODULE_MTD) || defined(DOXYGEN)
/**
 * @name    MTD device configuration
 *
 * Internal flash memory can be used as MTD device. For that purpose
 * a system MTD device has to be defined.
 * @{
 */

/** Default MTD device definition */
#define MTD_0 mtd_dev_get(0)

/**
 * @brief   MTD offset for SD Card interfaces
 *
 * MTD_1 is used for SD Card.
 */
#ifndef CONFIG_SDCARD_GENERIC_MTD_OFFSET
#define CONFIG_SDCARD_GENERIC_MTD_OFFSET    1
#endif

/** @} */
#endif /* defined(MODULE_MTD) || defined(DOXYGEN) */

#if defined(MODULE_SPIFFS) || defined(DOXYGEN)
/**
 * @name    SPIFFS configuration
 *
 * Configuration of the SPIFFS that can be used on the system MTD device.
 * @{
 */
#define SPIFFS_ALIGNED_OBJECT_INDEX_TABLES 1
#define SPIFFS_READ_ONLY 0
#define SPIFFS_SINGLETON 0
#define SPIFFS_HAL_CALLBACK_EXTRA 1
#define SPIFFS_CACHE 1
/** @} */
#endif /* defined(MODULE_SPIFFS) || defined(DOXYGEN) */

/**
  * @brief Print the board configuration in a human readable format
  */
void board_print_config (void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include definitions for optional off-board hardware modules */
#include "board_modules.h"

/** @} */

#endif /* ESP_PLATFORM */
