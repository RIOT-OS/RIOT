/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_cc110x
 * @{
 *
 * @file
 * @brief     cc110x board specific configuration
 *
 * @author    Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifndef CC110X_PARAMS_H
#define CC110X_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for CC110X driver
 * @{
 */
#ifndef CC110X_PARAM_SPI
#define CC110X_PARAM_SPI            (SPI_DEV(0))
#endif
#ifndef CC110X_PARAM_CS
#define CC110X_PARAM_CS             (GPIO_PIN(0, 0))
#endif
#ifndef CC110X_PARAM_GDO0
#define CC110X_PARAM_GDO0           (GPIO_PIN(0, 1))
#endif
#ifndef CC110X_PARAM_GDO1
#define CC110X_PARAM_GDO1           (GPIO_PIN(0, 2))
#endif
#ifndef CC110X_PARAM_GDO2
#define CC110X_PARAM_GDO2           (GPIO_PIN(0, 3))
#endif

#define CC110X_PARAMS             { .spi  = CC110X_PARAM_SPI,  \
                                    .cs   = CC110X_PARAM_CS, \
                                    .gdo0 = CC110X_PARAM_GDO0, \
                                    .gdo1 = CC110X_PARAM_GDO1, \
                                    .gdo2 = CC110X_PARAM_GDO2  }
/**
 * @}
 */

/**
 * @name CC110X configuration
 */
const cc110x_params_t cc110x_params[] = {
    CC110X_PARAMS
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC110X_PARAMS_H */
/** @} */
