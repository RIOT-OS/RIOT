/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx Power management
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include <cc26xx_cc13xx.h>
#include <stdbool.h>

#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Power domains
 */
typedef enum {
    POWER_DOMAIN_PERIPHERALS, /**< Peripherals domain */
    POWER_DOMAIN_SERIAL, /**< Serial domain */
    POWER_DOMAIN_RFC, /**< RF Core domain */
    POWER_DOMAIN_CPU, /**< CPU domain */
    POWER_DOMAIN_VIMS, /**< VIMS domain */
} power_domain_t;

/**
 * @brief   Is power domain enabled?
 *
 * @param[in] domain The domain.
 *
 * @return true Is enabled.
 * @return false Is not enabled.
 */
bool power_is_domain_enabled(const power_domain_t domain);

/**
 * @brief   Enable the specified power domain.
 *
 * @param[in] domain The domain.
 */
void power_enable_domain(const power_domain_t domain);

/**
 * @brief   Enable GPIO clock
 */
void power_clock_enable_gpio(void);

/**
 * @brief   Enable General Purpose Timer clock
 *
 * @param[in] tim The timer to enable
 */
void power_clock_enable_gpt(uint32_t tim);

/**
 * @brief   Enable I2C clock
 */
void power_clock_enable_i2c(void);

/**
 * @brief   Enable UART clocks
 *
 * @param[in] uart The UART number
 */
void power_clock_enable_uart(uart_t uart);

/**
 * @brief   Disable UART clocks
 *
 * @param[in] uart The UART number
 */
void power_clock_disable_uart(uart_t uart);

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
