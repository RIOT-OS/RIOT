/*
 * Copyright (C) 2015 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_z1
 * @{
 *
 * @file
 * @brief     cc2420 board specific configuration
 *
 * @author    Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CC2420_PARAMS_H_
#define CC2420_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name cc2420 configuration
 */
static const  cc2420_params_t cc2420_params[] =
    {
        {
            .spi = CC2420_SPI,
            .spi_speed = CC2420_SPI_CLK,
            .cs_pin = CC2420_CS,
            .fifo_int_pin = CC2420_FIFO_INT,
            .fifop_int_pin = CC2420_FIFOP_INT,
            .power_pin = CC2420_POWER,
            .reset_pin = CC2420_RESET,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC2420_PARAMS_H_ */
/** @} */
