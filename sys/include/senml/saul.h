/*
 * SPDX-FileCopyrightText: 2021 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_senml_saul SenML SAUL
 * @ingroup     sys_senml
 * @brief       Functionality for reading @ref drivers_saul sensors as @ref sys_senml
 *
 * The `senml_saul` module contains functions for reading sensors using
 * @ref drivers_saul and converting them to @ref sys_senml_cbor.
 *
 * @{
 *
 * @file
 * @brief       Functionality for reading @ref drivers_saul sensors as @ref sys_senml
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#include <stdint.h>
#include "nanocbor/nanocbor.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Use floats instead of decimal types when encoding SAUL measurements.
 *
 * If this is set to `1` the @ref phydat_t values from SAUL are converted to
 * @ref senml_numeric_t using @ref phydat_to_senml_float.
 * Values are converted using @ref phydat_to_senml_decimal otherwise.
 */
#ifndef CONFIG_SENML_SAUL_USE_FLOATS
#define CONFIG_SENML_SAUL_USE_FLOATS    0
#endif

/**
 * @brief Encode a single @ref drivers_saul sensor as senml+cbor.
 *
 * @param enc NanoCBOR encoder.
 * @param dev SAUL sensor to encode.
 *
 * @return Number of dimensions encoded. Less or equal to 0 on error.
 */
int senml_saul_reg_encode_cbor(nanocbor_encoder_t *enc, saul_reg_t *dev);

/**
 * @brief Encode all sensors from a @ref drivers_saul registry as senml+cbor.
 *
 * This functions reads all @ref drivers_saul sensors in a registry and encodes
 * the values as SenML/CBOR.
 *
 * @param buf Buffer to store the CBOR in.
 * @param len Length of the buffer.
 * @param reg SAUL registry to encode.
 *
 * @return Size of the encoded data.
 */
size_t senml_saul_encode_cbor(uint8_t *buf, size_t len, saul_reg_t *reg);

#ifdef __cplusplus
}
#endif

/** @} */
