/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef SOFT_UART_PARAMS_H
#define SOFT_UART_PARAMS_H

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
#endif /* SOFT_UART_PARAMS_H */
