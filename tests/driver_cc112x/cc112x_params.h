/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_at86rf2xx
 * @brief       generic at86rf231 pin config
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CC112x_PARAMS_H
#define CC112X_PARAMS_H

#include "cc112x.h"

/**
 * @brief make sure the SPI port and the needed GPIO pins are defined
 * @{
 */
#ifndef CC_SPI
#error "SPI pin not defined"
#endif
#ifndef CC_CS
#error "CS pin not defined"
#endif
#ifndef CC_GPIO0
#error "GPIO0 pin not defined"
#endif
#ifndef CC_GPIO1
#error "GPIO1 pin not defined"
#endif
#ifndef CC_GPIO2
#error "GPIO2 pin not defined"
#endif
#ifndef CC_GPIO3
#error "GPIO3 pin not defined"
#endif
#ifndef CC_RESET
#error "Reset pin not defined"
#endif
/**@}*/



#include "cc112x.h"
#include "board.h"
#include "periph_cpu.h"

/**
 * @name CC112X configuration
 */
const cc112x_params_t cc112x_params[] = {
    {
        .spi  = CC_SPI,
        .cs   = CC_CS,
        .gpio0 = CC_GPIO0,
		.gpio1 = CC_GPIO1,
        .gpio2 = CC_GPIO2,
		.gpio3 = CC_GPIO3,
		.reset = CC_RESET
    },
};


#ifdef __cplusplus
}
#endif

#endif /* CC1125_PARAMS_H_ */
