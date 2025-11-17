/*
 * SPDX-FileCopyrightText: 2024 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests_periph_uart_locate_pins
 * @{
 *
 * @file
 * @brief       Override "soft_uart_params.h" to conserve memory when
 *              changing the pin configuration
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "soft_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sotware UART port descriptor array
 */
extern soft_uart_conf_t soft_uart_config[];

#define SOFT_UART_NUMOF 1

#ifdef __cplusplus
}
#endif

/** @} */
