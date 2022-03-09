/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "senml.h"
#include "senml/cbor.h"
#include "nanocbor/nanocbor.h"

static int senml_encode_numeric_cbor(nanocbor_encoder_t *enc, const senml_numeric_t *v)
{
    switch (v->type) {
    case SENML_TYPE_NUMERIC_FLOAT:
        return nanocbor_fmt_float(enc, v->value.f);
    case SENML_TYPE_NUMERIC_DOUBLE:
        return nanocbor_fmt_double(enc, v->value.d);
    case SENML_TYPE_NUMERIC_INT:
        return nanocbor_fmt_int(enc, v->value.i);
    case SENML_TYPE_NUMERIC_UINT:
        return nanocbor_fmt_uint(enc, v->value.u);
    case SENML_TYPE_NUMERIC_DECFRAC:
        return nanocbor_fmt_decimal_frac(enc, v->value.df.e, v->value.df.m);
    default:
        return 0;
    }
}

static int senml_encode_start_cbor(nanocbor_encoder_t *enc, const senml_attr_t *attr,
                                   bool sum_value)
{
    int n = nanocbor_fmt_map(enc, !sum_value
                             + (attr->base_name != NULL)
                             + (attr->base_time.value.u != 0)
                             + (attr->base_unit != SENML_UNIT_NONE)
                             + (attr->base_value.value.u != 0)
#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM)
                             + (attr->base_sum.value.u != 0)
#endif
#if IS_ACTIVE(CONFIG_SENML_ATTR_VERSION)
                             + (attr->base_version != 0 && attr->base_version != 10)
#endif
                             + (attr->name != NULL)
                             + (attr->unit != SENML_UNIT_NONE)
#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM)
                             + (sum_value || attr->sum.value.u != 0)
#endif
                             + (attr->time.value.u != 0)
#if IS_ACTIVE(CONFIG_SENML_ATTR_UPDATE_TIME)
                             + (attr->update_time.value.u != 0)
#endif
                             );

    if (attr->base_name != NULL) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_NAME);
        n += nanocbor_put_tstr(enc, attr->base_name);
    }

    if (attr->base_time.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_TIME);
        n += senml_encode_numeric_cbor(enc, &attr->base_time);
    }

    if (attr->base_unit != SENML_UNIT_NONE) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_UNIT);
        n += nanocbor_put_tstr(enc, senml_unit_to_str(attr->base_unit));
    }

    if (attr->base_value.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_VALUE);
        n += senml_encode_numeric_cbor(enc, &attr->base_value);
    }

#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM)
    if (attr->base_sum.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_SUM);
        n += senml_encode_numeric_cbor(enc, &attr->base_sum);
    }
#endif

#if IS_ACTIVE(CONFIG_SENML_ATTR_VERSION)
    if (attr->base_version != 0 && attr->base_version != 10) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_BASE_VERSION);
        n += nanocbor_fmt_uint(enc, attr->base_version);
    }
#endif

    if (attr->name != NULL) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_NAME);
        n += nanocbor_put_tstr(enc, attr->name);
    }

    if (attr->unit != SENML_UNIT_NONE) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_UNIT);
        n += nanocbor_put_tstr(enc, senml_unit_to_str(attr->unit));
    }

#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM)
    if (sum_value || attr->sum.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_SUM);
        n += senml_encode_numeric_cbor(enc, &attr->sum);
    }
#endif

    if (attr->time.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_TIME);
        n += senml_encode_numeric_cbor(enc, &attr->time);
    }

#if IS_ACTIVE(CONFIG_SENML_ATTR_UPDATE_TIME)
    if (attr->update_time.value.u != 0) {
        n += nanocbor_fmt_int(enc, SENML_LABEL_UPDATE_TIME);
        n += senml_encode_numeric_cbor(enc, &attr->update_time);
    }
#endif

    return n;
}

#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM)
int senml_encode_sum_cbor(nanocbor_encoder_t *enc, const senml_attr_t *attr)
{
    return senml_encode_start_cbor(enc, attr, true);
}
#endif

int senml_encode_bool_cbor(nanocbor_encoder_t *enc, const senml_bool_value_t *val)
{
    return senml_encode_start_cbor(enc, &val->attr, false) +
           nanocbor_fmt_int(enc, SENML_LABEL_BOOLEAN_VALUE) +
           nanocbor_fmt_bool(enc, val->value);
}

int senml_encode_value_cbor(nanocbor_encoder_t *enc, const senml_value_t *val)
{
    return senml_encode_start_cbor(enc, &val->attr, false) +
           nanocbor_fmt_int(enc, SENML_LABEL_VALUE) +
           senml_encode_numeric_cbor(enc, &val->value);
}

int senml_encode_string_cbor(nanocbor_encoder_t *enc, const senml_string_value_t *val)
{
    return senml_encode_start_cbor(enc, &val->attr, false) +
           nanocbor_fmt_int(enc, SENML_LABEL_STRING_VALUE) +
           nanocbor_put_tstrn(enc, val->value, val->len);
}

int senml_encode_data_cbor(nanocbor_encoder_t *enc, const senml_data_value_t *val)
{
    return senml_encode_start_cbor(enc, &val->attr, false) +
           nanocbor_fmt_int(enc, SENML_LABEL_DATA_VALUE) +
           nanocbor_put_bstr(enc, val->value, val->len);
}
