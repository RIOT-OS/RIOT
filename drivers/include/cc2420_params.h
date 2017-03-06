/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 *
 * @{
 * @file
 * @brief       Default configuration for the CC2420 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CC2420_PARAMS_H
#define CC2420_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the CC2420 driver
 * @{
 */
#ifndef CC2420_PARAM_SPI
#define CC2420_PARAM_SPI        (SPI_DEV(0))
#endif
#ifndef CC2420_PARAM_SPI_CLK
#define CC2420_PARAM_SPI_CLK    (SPI_CLK_5MHZ)
#endif
#ifndef CC2420_PARAM_CS
#define CC2420_PARAM_CS         (GPIO_PIN(0, 0))
#endif
#ifndef CC2420_PARAM_FIFO
#define CC2420_PARAM_FIFO       (GPIO_PIN(0, 1))
#endif
#ifndef CC2420_PARAM_FIFOP
#define CC2420_PARAM_FIFOP      (GPIO_PIN(0, 2))
#endif
#ifndef CC2420_PARAM_CCA
#define CC2420_PARAM_CCA        (GPIO_PIN(0, 3))
#endif
#ifndef CC2420_PARAM_SFD
#define CC2420_PARAM_SFD        (GPIO_PIN(0, 3))
#endif
#ifndef CC2420_PARAM_VREFEN
#define CC2420_PARAM_VREFEN     (GPIO_PIN(0, 3))
#endif
#ifndef CC2420_PARAM_RESET
#define CC2420_PARAM_RESET      (GPIO_PIN(0, 3))
#endif

#define CC2420_PARAMS_DEFAULT   {.spi        = CC2420_PARAM_SPI, \
                                 .spi_clk    = CC2420_PARAM_SPI_CLK, \
                                 .pin_cs     = CC2420_PARAM_CS, \
                                 .pin_fifo   = CC2420_PARAM_FIFO, \
                                 .pin_fifop  = CC2420_PARAM_FIFOP, \
                                 .pin_cca    = CC2420_PARAM_CCA, \
                                 .pin_sfd    = CC2420_PARAM_SFD, \
                                 .pin_vrefen = CC2420_PARAM_VREFEN, \
                                 .pin_reset  = CC2420_PARAM_RESET}
/**@}*/

/**
 * @brief   CC2420 configuration
 */
static const cc2420_params_t cc2420_params[] =
{
#ifdef CC2420_PARAMS_BOARD
    CC2420_PARAMS_BOARD,
#else
    CC2420_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* CC2420_PARAMS_H */
/** @} */
