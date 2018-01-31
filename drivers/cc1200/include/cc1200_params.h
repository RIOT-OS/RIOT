/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_remote_revb
 * @{
 *
 * @file
 * @brief     cc1200 board specific configuration
 *
 * @author    Anon Mall <anon.mall@mail.gt-arc.com>
 */

#ifndef CC1200_PARAMS_H
#define CC1200_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

/**
 * @name    Set default configuration parameters for the CC1200 driver
 * @{
 */
#ifndef CC1200_PARAM_SPI
#define CC1200_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef CC1200_PARAM_SPI_CLK
#define CC1200_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif
#ifndef CC1200_PARAM_CS
#define CC1200_PARAM_CS          (GPIO_PIN(1, 5))
#endif
#ifndef CC1200_PARAM_INT
#define CC1200_PARAM_INT         (GPIO_PIN(1, 0))
#endif
#ifndef CC1200_PARAM_RESET
#define CC1200_PARAM_RESET       (GPIO_PIN(2, 7))
#endif

#define CC1200_PARAMS_DEFAULT       { .spi = CC1200_PARAM_SPI, \
                                      .spi_clk = CC1200_PARAM_SPI_CLK, \
                                      .cs_pin = CC1200_PARAM_CS, \
                                      .int_pin = CC1200_PARAM_INT, \
                                      .reset_pin = CC1200_PARAM_RESET }

/**
 * @name CC1200 default configuration
 */
static const cc1200_params_t cc1200_params[] =
{
#ifdef CC1200_PARAMS_BOARD
    CC1200_PARAMS_BOARD,
#else
    CC1200_PARAMS_DEFAULT,
#endif
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC1200_PARAMS_H */
/** @} */
