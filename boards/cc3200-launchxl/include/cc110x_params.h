/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_cc3200
 * @{
 *
 * @file
 * @brief     cc110x board specific configuration
 *
 * @author    Attilio Dona'
 */

#ifndef CC110X_PARAMS_H
#define CC110X_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CC110X configuration
 */
const cc110x_params_t cc110x_params[] = {
    {
        .spi  = SPI_0,
        .cs   = SPI_0_PIN_CS,
        .gdo0 = 19,
        .gdo1 = SPI_0_PIN_MISO, // miso pin
        .gdo2 = 2
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC110X_PARAMS_H */
/** @} */
