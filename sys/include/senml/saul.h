/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef SENML_SAUL_H
#define SENML_SAUL_H

#include <stdint.h>

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
#ifndef SENML_SAUL_USE_FLOATS
#define SENML_SAUL_USE_FLOATS (0)
#endif

/**
 * @brief Encode all sensors from @ref drivers_saul as senml+cbor.
 *
 * This functions reads all available @ref drivers_saul sensors and encodes the
 * values as SenML/CBOR.
 *
 * @param buf Buffer to store the CBOR in.
 * @param len Length of the buffer.
 *
 * @return Size of the encoded data.
 */
size_t senml_saul_encode_cbor(uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SENML_SAUL_H */
/** @} */
