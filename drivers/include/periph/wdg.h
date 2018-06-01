/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 * @file		wdg.h
 * @brief       Common watchdog peripheral interface
 * @author      EP [ep@unwds.com]
 */
#ifndef DRIVERS_INCLUDE_PERIPH_WDG_H_
#define DRIVERS_INCLUDE_PERIPH_WDG_H_

#include <stdbool.h>

/**
 * @brief Sets watchdog timer prescaler.
 *
 * @param prescaler watchdog timer prescaler value
 */
void wdg_set_prescaler(uint8_t prescaler);

/**
 * @brief Sets watchdog timer reload value.
 *
 * @param reload watchdog timer reload value
 */
void wdg_set_reload(uint16_t reload);

/**
 * @brief Enables watchdog timer. Once enabled, it cannot be disabled.
 */
void wdg_enable(void);

/**
 * @brief Reloads watchdog timer.
 *
 * @note Reload should be called by application before timer overflows, or watchdog timer will cause general reset.
 */
void wdg_reload(void);

/**
 * @brief Indicates that previous device reset was caused by watchdog.
 *
 * @return true if previous reset was caused by watchdog, false otherwise
 */
bool wdg_reset_occurred(void);

/**
 * @brief Gets current reload value in watchdog timer.
 *
 * @return current reload value
 */
uint32_t wdg_get_value(void);

#endif /* DRIVERS_INCLUDE_PERIPH_WDG_H_ */
