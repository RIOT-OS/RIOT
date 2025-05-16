/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_w5500
 * @{
 *
 * @file
 * @brief       Default parameters for W5500 Ethernet devices
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */


#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the W5500 driver
 * @{
 */
#ifndef W5500_PARAM_SPI
#define W5500_PARAM_SPI         (SPI_DEV(0))       /**< Default SPI device */
#endif
#ifndef W5500_PARAM_SPI_CLK
#define W5500_PARAM_SPI_CLK     (SPI_CLK_10MHZ)     /**< Default SPI speed */
#endif
#ifndef W5500_PARAM_CS
#define W5500_PARAM_CS          (GPIO_PIN(0, 27))   /**< Default SPI chip select pin */
#endif
#ifndef W5500_PARAM_INT
#define W5500_PARAM_INT         GPIO_UNDEF          /**< set to invalid */
#endif
#ifndef CONFIG_W5500_POLLING_INTERVAL
#define CONFIG_W5500_POLLING_INTERVAL 100u          /**< default polling interval 100 ms */
#endif

#ifndef W5500_PARAMS
/**
 * @brief   W5500 initialization parameters
 */

#define W5500_PARAMS   { .spi = W5500_PARAM_SPI,     \
                         .clk = W5500_PARAM_SPI_CLK, \
                         .cs = W5500_PARAM_CS,       \
                         .irq = W5500_PARAM_INT,     \
                         .polling_interval_ms = CONFIG_W5500_POLLING_INTERVAL }
#endif
/** @} */

/**
 * @brief   W5500 configuration
 */
static const w5500_params_t w5500_params[] = {
    W5500_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
