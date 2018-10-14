/*
 * Copyright (C) 2018 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_il3829
 *
 * @{
 * @file
 * @brief       Default configuration for il3829
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#ifndef IL3829_PARAMS_H
#define IL3829_PARAMS_H

#include "board.h"
#include "il3829.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for IL3829 display */
/**
 * @name    Set default configuration parameters for the IL3829
 * @{
 */
#ifndef IL3829_PARAM_SPI
#define IL3829_PARAM_SPI          (SPI_DEV(0))
#endif
#ifndef IL3829_PARAM_SPI_CLK
#define IL3829_PARAM_SPI_CLK      (SPI_CLK_5MHZ)
#endif
#ifndef IL3829_PARAM_CS
#define IL3829_PARAM_CS           (GPIO_UNDEF)
#endif
#ifndef IL3829_PARAM_DC
#define IL3829_PARAM_DC           (GPIO_UNDEF)
#endif
#ifndef IL3829_PARAM_RST
#define IL3829_PARAM_RST          (GPIO_UNDEF)
#endif
#ifndef IL3829_PARAM_BUSY
#define IL3829_PARAM_BUSY         (GPIO_UNDEF)
#endif

#ifndef IL3829_PARAMS
#define IL3829_PARAMS              { .spi = IL3829_PARAM_SPI, \
                                     .spi_clk = IL3829_PARAM_SPI_CLK, \
                                     .cs_pin = IL3829_PARAM_CS, \
                                     .dc_pin = IL3829_PARAM_DC, \
                                     .rst_pin = IL3829_PARAM_RST, \
                                     .busy_pin = IL3829_PARAM_BUSY, \
                                   }
#endif
/**@}*/

/**
 * @brief   Configure IL3829
 */
static const spi_display_params_t il3829_params[] =
{
    IL3829_PARAMS,
};

#ifdef __cplusplus
}
#endif
#endif /* IL3829_PARAMS_H */
