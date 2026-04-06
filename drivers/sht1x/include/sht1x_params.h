/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sht1x
 *
 * @{
 * @file
 * @brief       Default configuration for SHT10/SHT11/SHT15 devices
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

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
/**
 * @brief   SHT1x the clock pin
 */
#define SHT1X_PARAM_CLK               (GPIO_PIN(1, 25))
#endif
#ifndef SHT1X_PARAM_DATA
/**
 * @brief   SHT1x the data pin
 */
#define SHT1X_PARAM_DATA              (GPIO_PIN(1, 26))
#endif
#ifndef SHT1X_PARAM_VDD
/**
 * @brief   SHT1x the supply voltage
 *
 * @see sht1x_vdd_t
 */
#define SHT1X_PARAM_VDD               (SHT1X_VDD_3_5V)
#endif
#ifndef SHT1X_PARAMS
/**
 * @brief   The initialization parameters for the SHT1x driver
 */
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

/** @} */
