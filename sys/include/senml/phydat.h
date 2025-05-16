/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_senml_phydat SenML Phydat
 * @ingroup     sys_senml
 * @brief       Functionality for converting from @ref sys_phydat to @ref sys_senml
 *
 * The `senml_phydat` module contains various functions for converting
 * @ref sys_phydat values to @ref sys_senml.
 *
 * @{
 *
 * @file
 * @brief       Functionality for converting from @ref sys_phydat to @ref sys_senml
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */


#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "senml.h"
#include <phydat.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a SenML boolean value.
 *
 * Writes the value of the given @p dim of @p phydat as a boolean.
 * @p phydat is assumed to be of @ref UNIT_BOOL.
 *
 * @note `phydat->scale` must be zero.
 *
 * @param[out] senml       SenML value to store value in.
 * @param[in]  phydat      Phydat to convert.
 * @param[in]  dim         Dimension of @p phydat to convert.
 */
void phydat_to_senml_bool(senml_bool_value_t *senml, const phydat_t *phydat, const uint8_t dim);

/**
 * @brief Create a SenML float value.
 *
 * Writes the value of the given @p dim of @p phydat as a float.
 * The unit of @p phydat is converted using the following rules:
 *
 * - @ref UNIT_TIME is converted to @ref SENML_UNIT_SECOND.
 * - @ref UNIT_F is converted to @ref SENML_UNIT_KELVIN.
 * - @ref UNIT_G_FORCE is converted to @ref SENML_UNIT_METER_PER_SQUARE_SECOND.
 * - @ref UNIT_BAR is converted to @ref SENML_UNIT_PASCAL.
 * - @ref UNIT_GPM3 is converted to @ref SENML_UNIT_KILOGRAM_PER_CUBIC_METER.
 * - @ref UNIT_GAUSS is converted to @ref SENML_UNIT_TESLA.
 * - Compatible units are set to their SenML equivalent.
 * - Incompatible (or unknown) units are set to @ref SENML_UNIT_NONE.
 *
 * @param[out] senml       SenML value to store value in.
 * @param[in]  phydat      Phydat to convert.
 * @param[in]  dim         Dimension of @p phydat to convert.
 */
void phydat_to_senml_float(senml_value_t *senml, const phydat_t *phydat, const uint8_t dim);

/**
 * @brief Create a SenML decimal fraction value.
 *
 * Writes the value of the given @p dim of @p phydat as a decimal value.
 * The unit of @p phydat is converted using the following rules:
 *
 * - @ref UNIT_TIME is converted to @ref SENML_UNIT_SECOND.
 * - @ref UNIT_BAR is converted to @ref SENML_UNIT_PASCAL.
 * - @ref UNIT_GPM3 is converted to @ref SENML_UNIT_KILOGRAM_PER_CUBIC_METER.
 * - @ref UNIT_GAUSS is converted to @ref SENML_UNIT_TESLA.
 * - Compatible units are set to their SenML equivalent.
 * - Incompatible (or unknown) units are set to @ref SENML_UNIT_NONE.
 *
 * @param[out] senml       SenML value to store value in.
 * @param[in]  phydat      Phydat to convert.
 * @param[in]  dim         Dimension of @p phydat to convert.
 */
void phydat_to_senml_decimal(senml_value_t *senml, const phydat_t *phydat, const uint8_t dim);

#ifdef __cplusplus
}
#endif

/** @} */
