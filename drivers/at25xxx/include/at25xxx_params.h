/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at25xxx
 * @{
 *
 * @file
 * @brief       Default configuration for the M95M01 EEPROM
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef AT25XXX_PARAMS_H
#define AT25XXX_PARAMS_H

#include "board.h"
#include "at25xxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT25XXX driver
 * @{
 */
#ifndef AT25XXX_PARAM_SPI
#define AT25XXX_PARAM_SPI       (SPI_DEV(0))
#endif
#ifndef AT25XXX_PARAM_SPI_CLK
#define AT25XXX_PARAM_SPI_CLK   (SPI_CLK_5MHZ)
#endif
#ifndef AT25XXX_PARAM_CS
#define AT25XXX_PARAM_CS        (GPIO_PIN(0, 0))
#endif
#ifndef AT25XXX_PARAM_WP
#define AT25XXX_PARAM_WP        (GPIO_UNDEF)
#endif
#ifndef AT25XXX_PARAM_HOLD
#define AT25XXX_PARAM_HOLD      (GPIO_UNDEF)
#endif
#ifndef AT25XXX_PARAM_SIZE
#define AT25XXX_PARAM_SIZE      (128 * 1024UL)  /* EEPROM size, in bytes */
#endif
#ifndef AT25XXX_PARAM_ADDR_LEN
#define AT25XXX_PARAM_ADDR_LEN          (24)    /* Address length, in bits */
#endif
#ifndef AT25XXX_PARAM_PAGE_SIZE
#define AT25XXX_PARAM_PAGE_SIZE        (256)    /* Page size, in bytes */
#endif

#ifndef AT25XXX_PARAMS
#define AT25XXX_PARAMS          { .spi = AT25XXX_PARAM_SPI,         \
                                  .spi_clk = AT25XXX_PARAM_SPI_CLK, \
                                  .cs_pin = AT25XXX_PARAM_CS,       \
                                  .wp_pin = AT25XXX_PARAM_WP,       \
                                  .hold_pin = AT25XXX_PARAM_HOLD,   \
                                  .size = AT25XXX_PARAM_SIZE,       \
                                  .page_size = AT25XXX_PARAM_PAGE_SIZE }
#endif
/** @} */

/**
 * @brief   AT25XXX configuration
 */
static const at25xxx_params_t at25xxx_params[] =
{
    AT25XXX_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* AT25XXX_PARAMS_H */
/** @} */
