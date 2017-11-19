/*
 * Copyright (C) 2017 Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_msbiot
 * @{
 *
 * @file
 * @brief     cc110x board specific configuration
 *
 * @author    Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CC110X_PARAMS_H
#define CC110X_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CC110X configuration
 */
const cc110x_params_t cc110x_params[] = {
    {
        .spi  = CC110X_SPI,
        .cs   = CC110X_CS,
        .gdo0 = CC110X_GDO0,
        .gdo1 = CC110X_GDO1,
        .gdo2 = CC110X_GDO2
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC110X_PARAMS_H */
/** @} */
