/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_radiolib
 * @{
 *
 * @file
 * @brief       Default configuration for the RadioLib RIOT HAL
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the RadioLib radio
 * @{
 */
/**
 * @brief   SPI bus the transceiver is connected to
 */
#ifndef CONFIG_RADIOLIB_SPI
#  ifdef SX127X_PARAM_SPI
#    define CONFIG_RADIOLIB_SPI SX127X_PARAM_SPI
#  else
#    define CONFIG_RADIOLIB_SPI SPI_UNDEF
#  endif
#endif

/**
 * @brief   SPI bus clock speed
 */
#ifndef CONFIG_RADIOLIB_SPI_CLK
#  define CONFIG_RADIOLIB_SPI_CLK SPI_CLK_5MHZ
#endif

/**
 * @brief   Chip select pin (NSS)
 */
#ifndef CONFIG_RADIOLIB_SPI_NSS
#  ifdef SX127X_PARAM_SPI_NSS
#    define CONFIG_RADIOLIB_SPI_NSS SX127X_PARAM_SPI_NSS
#  else
#    define CONFIG_RADIOLIB_SPI_NSS GPIO_UNDEF
#  endif
#endif

/**
 * @brief   Reset pin
 */
#ifndef CONFIG_RADIOLIB_RESET
#  ifdef SX127X_PARAM_RESET
#    define CONFIG_RADIOLIB_RESET SX127X_PARAM_RESET
#  else
#    define CONFIG_RADIOLIB_RESET GPIO_UNDEF
#  endif
#endif

/**
 * @brief   Interrupt pin
 */
#ifndef CONFIG_RADIOLIB_IRQ
#  ifdef SX127X_PARAM_DIO0
#    define CONFIG_RADIOLIB_IRQ SX127X_PARAM_DIO0
#  else
#    define CONFIG_RADIOLIB_IRQ GPIO_UNDEF
#  endif
#endif

/**
 * @brief   Auxiliary pin
 */
#ifndef CONFIG_RADIOLIB_GPIO
#  ifdef SX127X_PARAM_DIO1
#    define CONFIG_RADIOLIB_GPIO SX127X_PARAM_DIO1
#  else
#    define CONFIG_RADIOLIB_GPIO GPIO_UNDEF
#  endif
#endif

/**
 * @brief   Default carrier frequency in MHz
 */
#ifndef CONFIG_RADIOLIB_FREQ
#  define CONFIG_RADIOLIB_FREQ (868.0)
#endif
/**@}*/

#ifdef __cplusplus
}
#endif

/** @} */
