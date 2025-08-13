/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_senml_cbor SenML CBOR
 * @ingroup     sys_senml
 * @brief       Functionality for encoding SenML values as CBOR
 *
 * The `senml_cbor` module contains functionality for encoding @ref sys_senml
 * values to CBOR using @ref pkg_nanocbor.
 *
 * @{
 *
 * @file
 * @brief       Functionality for encoding SenML values as CBOR
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "senml.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SenML CBOR labels
 *
 * This list contains the SenML CBOR labels as assigned by IANA.
 */
typedef enum {
    SENML_LABEL_BASE_VERSION    = -1,
    SENML_LABEL_BASE_NAME       = -2,
    SENML_LABEL_BASE_TIME       = -3,
    SENML_LABEL_BASE_UNIT       = -4,
    SENML_LABEL_BASE_VALUE      = -5,
    SENML_LABEL_BASE_SUM        = -6,
    SENML_LABEL_NAME            = 0,
    SENML_LABEL_UNIT            = 1,
    SENML_LABEL_VALUE           = 2,
    SENML_LABEL_STRING_VALUE    = 3,
    SENML_LABEL_BOOLEAN_VALUE   = 4,
    SENML_LABEL_SUM             = 5,
    SENML_LABEL_TIME            = 6,
    SENML_LABEL_UPDATE_TIME     = 7,
    SENML_LABEL_DATA_VALUE      = 8,
} senml_cbor_label_t;

#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM) || defined(DOXYGEN)
/**
 * @brief Encode @ref senml_attr_t containing `sum` as CBOR.
 *
 * Requires the `sum` attribute to be enabled by setting `CONFIG_SENML_ATTR_SUM` to 1.
 *
 * @param enc NanoCBOR encoder.
 * @param attr Attributes (including `sum`) to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_sum_cbor(nanocbor_encoder_t *enc, const senml_attr_t *attr);
#endif

/**
 * @brief Encode @ref senml_bool_value_t as CBOR.
 *
 * @param enc NanoCBOR encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_bool_cbor(nanocbor_encoder_t *enc, const senml_bool_value_t *val);

/**
 * @brief Encode @ref senml_value_t as CBOR.
 *
 * @param enc NanoCBOR encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_value_cbor(nanocbor_encoder_t *enc, const senml_value_t *val);

/**
 * @brief Encode @ref senml_string_value_t as CBOR.
 *
 * @param enc NanoCBOR encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_string_cbor(nanocbor_encoder_t *enc, const senml_string_value_t *val);

/**
 * @brief Encode @ref senml_data_value_t as CBOR.
 *
 * @param enc NanoCBOR encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_data_cbor(nanocbor_encoder_t *enc, const senml_data_value_t *val);

#ifdef __cplusplus
}
#endif

/** @} */
