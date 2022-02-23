/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "nanocbor/nanocbor.h"
#include "saul_reg.h"
#include "senml.h"
#include "senml/cbor.h"
#include "senml/phydat.h"
#include "senml/saul.h"

static inline void senml_encode_phydat_bool(nanocbor_encoder_t *enc,
                                            const saul_reg_t *dev,
                                            const phydat_t *data,
                                            const uint8_t dim)
{
    senml_bool_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_bool(&val, data, dim);
    senml_encode_bool_cbor(enc, &val);
}

static inline uint8_t senml_fix_unit(const saul_reg_t *dev, const uint8_t unit)
{
    /* Fix the unit for relative humidity. */
    if (dev->driver->type == SAUL_SENSE_HUM &&
        unit == SENML_UNIT_PERCENT) {
        return SENML_UNIT_RELATIVE_HUMIDITY_PERCENT;
    }
    return unit;
}

static void senml_encode_phydat_float(nanocbor_encoder_t *enc,
                                      const saul_reg_t *dev,
                                      const phydat_t *data, const uint8_t dim)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_float(&val, data, dim);
    val.attr.unit = senml_fix_unit(dev, val.attr.unit);
    senml_encode_value_cbor(enc, &val);
}

static void senml_encode_phydat_decimal(nanocbor_encoder_t *enc,
                                        const saul_reg_t *dev,
                                        const phydat_t *data, const uint8_t dim)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_decimal(&val, data, dim);
    val.attr.unit = senml_fix_unit(dev, val.attr.unit);
    senml_encode_value_cbor(enc, &val);
}

int senml_saul_reg_encode_cbor(nanocbor_encoder_t *enc, saul_reg_t *dev)
{
    phydat_t data;
    int dim = saul_reg_read(dev, &data);

    if (dim <= 0) {
        return dim;
    }

    for (uint8_t i = 0; i < dim; i++) {
        if (data.unit == UNIT_BOOL) {
            senml_encode_phydat_bool(enc, dev, &data, i);
        }
        else if (CONFIG_SENML_SAUL_USE_FLOATS) {
            senml_encode_phydat_float(enc, dev, &data, i);
        }
        else {
            senml_encode_phydat_decimal(enc, dev, &data, i);
        }
    }

    return dim;
}

size_t senml_saul_encode_cbor(uint8_t *buf, size_t len, saul_reg_t *dev)
{
    nanocbor_encoder_t enc;

    nanocbor_encoder_init(&enc, buf, len);
    nanocbor_fmt_array_indefinite(&enc);

    while (dev) {
        if (senml_saul_reg_encode_cbor(&enc, dev) <= 0) {
            return 0;
        }
        dev = dev->next;
    }

    nanocbor_fmt_end_indefinite(&enc);
    return nanocbor_encoded_len(&enc);
}
