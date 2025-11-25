/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       cc110x board specific configuration
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CC110X_PARAMS_H
#define CC110X_PARAMS_H

#include "board.h"
#include "cc110x_settings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default parameters for the cc110x driver
 *
 * These values are based on the msba2 board
 * @{
 */
#ifndef CC110X_PARAM_SPI
#define CC110X_PARAM_SPI            SPI_DEV(0)      /**< SPI bus connected to CC110x */
#endif

#ifndef CC110X_PARAM_CS
#define CC110X_PARAM_CS             GPIO_PIN(1, 21) /**< SPI-CS connected to CC110x */
#endif

#ifndef CC110X_PARAM_GDO0
#define CC110X_PARAM_GDO0           GPIO_PIN(0, 27) /**< GPIO connected to CC110x' GDO0 pin */
#endif

#ifndef CC110X_PARAM_GDO2
#define CC110X_PARAM_GDO2           GPIO_PIN(0, 28) /**< GPIO connected to CC110x's GDO2 pin */
#endif

#ifndef CC110X_PARAM_SPI_CLOCK
#define CC110X_PARAM_SPI_CLOCK      SPI_CLK_5MHZ    /**< SPI clock frequency to use */
#endif

#ifndef CC110X_PARAM_PATABLE
/**
 * @brief   PA table to use
 *
 * Choose the one matching the base frequency your transceiver uses, otherwise
 * the TX power setting will be incorrect.
 */
#if IS_USED(MODULE_CC110X_433MHZ)
#define CC110X_PARAM_PATABLE        (&cc110x_patable_433mhz)
#elif IS_USED(MODULE_CC110X_868MHZ)
#define CC110X_PARAM_PATABLE        (&cc110x_patable_868mhz)
#elif IS_USED(MODULE_CC110X_915MHZ)
#define CC110X_PARAM_PATABLE        (&cc110x_patable_915mhz)
#endif
#endif

#ifndef CC110X_PARAM_CONFIG
/**
 * @brief   Default config to apply
 *
 * If 868 MHz is used as base frequency, you can set this to `NULL`
 */
#if IS_USED(MODULE_CC110X_433MHZ)
#define CC110X_PARAM_CONFIG         (&cc110x_config_433mhz_250kbps_300khz)
#else
#define CC110X_PARAM_CONFIG         NULL
#endif
#endif

#ifndef CC110X_PARAM_CHANNELS
/**
 * @brief   Default channel map to use
 *
 * This must match to configuration you have chosen
 */
#if IS_USED(MODULE_CC110X_433MHZ)
#define CC110X_PARAM_CHANNELS       (&cc110x_chanmap_433mhz_300khz)
#else
#define CC110X_PARAM_CHANNELS       (&cc110x_chanmap_868mhz_lora)
#endif
#endif

#ifndef CC110X_PARAMS
/**
 * @brief   Default initialization parameters of the CC110x driver
 */
#define CC110X_PARAMS               { \
        .spi      = CC110X_PARAM_SPI,  \
        .spi_clk  = CC110X_PARAM_SPI_CLOCK, \
        .cs       = CC110X_PARAM_CS,   \
        .gdo0     = CC110X_PARAM_GDO0, \
        .gdo2     = CC110X_PARAM_GDO2, \
        .patable  = CC110X_PARAM_PATABLE, \
        .config   = CC110X_PARAM_CONFIG, \
        .channels = CC110X_PARAM_CHANNELS, \
}

#endif
/** @} */

/**
 * @brief   CC110X initialization parameters
 */
static const cc110x_params_t cc110x_params[] = {
    CC110X_PARAMS
};

#ifdef __cplusplus
}
#endif
#endif /* CC110X_PARAMS_H */
/** @} */
