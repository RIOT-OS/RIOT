/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht1x
 *
 * @{
 * @file
 * @brief       Default configuration for SHT10/SHT11/SHT15 devices
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SHT1X_PARAMS_H
#define SHT1X_PARAMS_H

#include "board.h"
#include "sht1x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SHT1X devices
 * @{
 */
#ifndef SHT1X_PARAM_CLK
#define SHT1X_PARAM_CLK               (GPIO_PIN(1, 25))
#endif
#ifndef SHT1X_PARAM_DATA
#define SHT1X_PARAM_DATA              (GPIO_PIN(1, 26))
#endif
#ifndef SHT1X_PARAM_VDD
#define SHT1X_PARAM_VDD               (SHT1X_VDD_3_5V)
#endif
#ifndef SHT1X_PARAMS
#define SHT1X_PARAMS                  { .clk = SHT1X_PARAM_CLK,  \
                                        .data = SHT1X_PARAM_DATA, \
                                        .vdd = SHT1X_PARAM_VDD }
#endif
/**@}*/

/**
 * @brief   Configure SHT1X devices
 */
static const sht1x_params_t sht1x_params[] =
{
    SHT1X_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SHT1X_PARAMS_H */
/** @} */
