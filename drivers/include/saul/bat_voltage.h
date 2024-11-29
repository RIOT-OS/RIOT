/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       Parameter definitions for mapping battery voltage to SAUL
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 */

#ifndef SAUL_BAT_VOLTAGE_H
#define SAUL_BAT_VOLTAGE_H

#include <stdint.h>

#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SAUL battery voltage configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    int8_t phydat_scale;    /**< Phydat scale of the resulting voltage */
    adc_t line;             /**< ADC line to initialize and expose */
    adc_res_t res;          /**< ADC resolution */
    /**
     * @brief   Conversion function to convert raw ADC data to voltage
     *
     * @param[in] adc_sample    The raw ADC sample.
     *
     * @return Voltage value for phydat.
     */
    int16_t (*convert)(int32_t adc_sample);
} saul_bat_voltage_params_t;

#ifdef __cplusplus
}
#endif

#endif /* SAUL_BAT_VOLTAGE_H */
/** @} */
