/*
 * Copyright (C) Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rfr2
 *
 * @{
 * @file
 * @brief       Default configuration for the AT86RF2xx drive
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef AT86RFR2_PARAMS_H
#define AT86RFR2_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the AT86RFR2 SOC driver
 * @{
 */
#define AT86RFR2_PARAMS_DEFAULT    {.spi = 0, \
                                     .spi_speed = 0x86F2, \
                                     .cs_pin = 0, \
                                     .int_pin = 0, \
                                     .sleep_pin = 0, \
                                     .reset_pin = 0}
/**@}*/

/**
 * @brief   AT86RF231 configuration
 */
static const at86rf2xx_params_t at86rfr2_params[] =
{
		AT86RFR2_PARAMS_DEFAULT,
};

#ifdef __cplusplus
}
#endif

#endif /* AT86RFR2_PARAMS_H */
/** @} */
