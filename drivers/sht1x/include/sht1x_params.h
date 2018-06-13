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
 * @name    Set default SAUL info text depending on used pseudo module
 * @{
 */
#ifndef SHT1X_SAULINFO
#ifdef MODULE_SHT15
#define SHT1X_SAULINFO                { .name = "SHT15 temperature" }, \
                                      { .name = "SHT15 humidity" }
#else
#ifdef MODULE_SHT10
#define SHT1X_SAULINFO                { .name = "SHT10 temperature" }, \
                                      { .name = "SHT10 humidity" }
#else
/* SHT11 is the most commonly used, so use that as default */
#define SHT1X_SAULINFO                { .name = "SHT11 temperature" }, \
                                      { .name = "SHT11 humidity" }
#endif /* MODULE_SHT10 */
#endif /* MODULE_SHT15 */
#endif /* SHT1X_SAULINFO */

/**@}*/

/**
 * @brief   Configure SHT1X devices
 */
static const sht1x_params_t sht1x_params[] =
{
    SHT1X_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t sht1x_saul_info[] =
{
    SHT1X_SAULINFO
};

#ifdef __cplusplus
}
#endif

#endif /* SHT1X_PARAMS_H */
/** @} */
