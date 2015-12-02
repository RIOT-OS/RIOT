/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_msba2
 * @{
 *
 * @file
 * @brief     cc110x board specific configuration
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC110X_PARAMS_H
#define CC110X_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define USE_BOARD_PARAMETERS

#define CC_SPI		SPI_EXT
#define CC_CS		GPIO_T(GPIO_PORT_E, 13)
#define CC_GPIO0	GPIO_T(GPIO_PORT_D, 14)
#define CC_GPIO1	GPIO_T(GPIO_PORT_E, 11)
#define CC_GPIO2	GPIO_T(GPIO_PORT_A, 7)

/**
 * @name CC110X configuration
 */
const cc110x_params_t cc110x_params[] = {
    {
        .spi  = CC_SPI,
        .cs   = CC_CS,
        .gdo0 = CC_GPIO0,
        .gdo1 = CC_GPIO1,
        .gdo2 = CC_GPIO2
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC110X_PARAMS_H */
/** @} */
