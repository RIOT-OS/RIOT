/*
 * SPDX-FileCopyrightText: 2024 Martine S. Lenders
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <mail@martine-lenders.eu>
 */

#include "net/skald/bthome.h"

static int _add_50perc_to_binary_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_button_press(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_int8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_int16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_uint8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_10_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_10_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_100_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_100_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_1000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_div_10000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_g_force_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_ppb_to_ugm3_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_10_uint8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_10_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_10_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_100_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_100_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_1000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);
static int _add_times_10000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx);

static int _saul_sense_to_bthome_id(skald_bthome_saul_t *saul, const phydat_t *data)
{
    switch (saul->saul.driver->type & SAUL_ID_MASK) {
    case SAUL_SENSE_ID_ANY:
        return -1;
    case SAUL_SENSE_ID_BTN:
        saul->obj_id = BTHOME_ID_BUTTON;
        saul->add_measurement = &_add_button_press;
        return 0;
    case SAUL_SENSE_ID_TEMP:
    case SAUL_SENSE_ID_OBJTEMP:
        switch (data->unit) {
        case UNIT_TEMP_C:
            switch (data->scale) {
            case -2:
                saul->obj_id = BTHOME_ID_TEMPERATURE_FACTOR_0_01;
                saul->add_measurement = &_add_int16_measurement;
                return 0;
            case -1:
                saul->obj_id = BTHOME_ID_TEMPERATURE_FACTOR_0_1;
                saul->add_measurement = &_add_int16_measurement;
                return 0;
            case 0:
                saul->obj_id = BTHOME_ID_TEMPERATURE_FACTOR_1;
                saul->add_measurement = &_add_int8_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_HUM:
        switch (data->unit) {
        case UNIT_PERCENT:
            switch (data->scale) {
            case -2:
                saul->obj_id = BTHOME_ID_HUMIDITY_FACTOR_0_01;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            case -1:
                saul->obj_id = BTHOME_ID_HUMIDITY_FACTOR_0_01;
                saul->add_measurement = &_add_times_10_uint16_measurement;
                return 0;
            case 0:
                saul->obj_id = BTHOME_ID_HUMIDITY_FACTOR_1;
                saul->add_measurement = &_add_uint8_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_LIGHT:
        switch (data->unit) {
        case UNIT_LUX:
            switch (data->scale) {
            case 0:
                saul->obj_id = BTHOME_ID_ILLUMINANCE_FACTOR_0_01;
                saul->add_measurement = &_add_times_100_uint24_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_ACCEL:
        switch (data->unit) {
        case UNIT_G_FORCE:
            switch (data->scale) {
            case -3:
                saul->obj_id = BTHOME_ID_ACCELERATION_FACTOR_0_001;
                saul->add_measurement = &_add_g_force_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_MAG:
        return -1;
    case SAUL_SENSE_ID_GYRO:
        switch (data->unit) {
        case UNIT_DPS:
            saul->obj_id = BTHOME_ID_GYROSCOPE_FACTOR_0_001;
            switch (data->scale) {
            case -1:
                saul->add_measurement = &_add_times_100_uint16_measurement;
                return 0;
            case 0:
                saul->add_measurement = &_add_times_1000_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_COLOR:
        return -1;
    case SAUL_SENSE_ID_PRESS:
        switch (data->unit) {
        case UNIT_BAR:
            switch (data->scale) {
            case -6:
                saul->obj_id = BTHOME_ID_PRESSURE_FACTOR_0_01;
                /* 10^-6 bar = 0.000001 bar = 0.1 Pa = 0.001 hPa
                 * => data / 10 = 10^-6 bar in 0.01 hPA */
                saul->add_measurement = &_add_div_10_uint24_measurement;
                return 0;
            default:
                return -1;
            }
        case UNIT_PA:
            saul->obj_id = BTHOME_ID_PRESSURE_FACTOR_0_01;
            switch (data->scale) {
            case -2:
                /* 10^-2 Pa = 0.0001 hPa
                 * => data / 100 = 10^-2 Pa in 0.01 hPa */
                saul->add_measurement = &_add_div_100_uint24_measurement;
                return 0;
            case 0:
                saul->add_measurement = &_add_uint24_measurement;
                return 0;
            case 1:
                /* 10^1 Pa = 0.1 hPa */
                saul->add_measurement = &_add_times_10_uint24_measurement;
                return 0;
            case 2:
                /* 10^2 Pa = 1 hPa */
                saul->add_measurement = &_add_times_100_uint24_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
    case SAUL_SENSE_ID_ANALOG:
        return -1;
    case SAUL_SENSE_ID_UV:
        switch (data->scale) {
        case -1:
            saul->obj_id = BTHOME_ID_UV_INDEX_FACTOR_0_1;
            saul->add_measurement = &_add_uint8_measurement;
            return 0;
        case 0:
            saul->obj_id = BTHOME_ID_UV_INDEX_FACTOR_0_1;
            saul->add_measurement = &_add_times_10_uint8_measurement;
            return 0;
        default:
            return -1;
        }
    case SAUL_SENSE_ID_COUNT:
        switch (data->unit) {
        case UNIT_CPM3:
            switch (data->scale) {
            case 3:
                saul->obj_id = BTHOME_ID_COUNT_UINT32;
                saul->add_measurement = &_add_times_1000_uint16_measurement;
                return 0;
            case 4:
                saul->obj_id = BTHOME_ID_COUNT_UINT32;
                saul->add_measurement = &_add_times_10000_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        case UNIT_NONE:
            switch (data->scale) {
            case 0:
                saul->obj_id = BTHOME_ID_COUNT_UINT16;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
        return -1;
    case SAUL_SENSE_ID_DISTANCE:
        switch (data->unit) {
        case UNIT_M:
            switch (data->scale) {
            case -3:
                saul->obj_id = BTHOME_ID_DISTANCE_MM;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            case 0:
                saul->obj_id = BTHOME_ID_DISTANCE_M;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_CO2:
        switch (data->unit) {
        case UNIT_PPM:
            saul->obj_id = BTHOME_ID_CO2;
            switch (data->scale) {
            case -2:
                saul->add_measurement = &_add_div_100_uint16_measurement;
                return 0;
            case 0:
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_TVOC:
        switch (data->unit) {
        case UNIT_PPB:
            switch (data->scale) {
            case 0:
                saul->obj_id = BTHOME_ID_TVOC;
                saul->add_measurement = &_add_ppb_to_ugm3_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_GAS:
        /* only gas sensor so far seems to be a poti that outputs in Ohm */
        return -1;
    case SAUL_SENSE_ID_OCCUP:
        switch (data->unit) {
        case UNIT_PERCENT:
            switch (data->scale) {
            case 0:
                saul->obj_id = BTHOME_ID_OCCUPANCY_BINARY;
                saul->add_measurement = &_add_50perc_to_binary_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_PROXIMITY:
        /* proximity sensors return in counts. Don't know how to convert */
        return -1;
    case SAUL_SENSE_ID_RSSI:
        return -1;
    case SAUL_SENSE_ID_CHARGE:
        /* none of the sensors express the charge in % as, e.g., BTHOME_ID_BATTERY */
        return -1;
    case SAUL_SENSE_ID_CURRENT:
        switch (data->unit) {
        case UNIT_A:
            switch (data->scale) {
            case -6:
                saul->obj_id = BTHOME_ID_CURRENT_UINT_FACTOR_0_001;
                saul->add_measurement = &_add_div_1000_uint16_measurement;
                return 0;
            case -5:
                saul->obj_id = BTHOME_ID_CURRENT_UINT_FACTOR_0_001;
                saul->add_measurement = &_add_div_100_uint16_measurement;
                return 0;
            case -4:
                saul->obj_id = BTHOME_ID_CURRENT_UINT_FACTOR_0_001;
                saul->add_measurement = &_add_div_10_uint16_measurement;
                return 0;
            case -3:
                saul->obj_id = BTHOME_ID_CURRENT_UINT_FACTOR_0_001;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_PM:
        /* Left as exercise to the reader to convert it to either
         * BTHOME_ID_PM_2_5 or BTHOME_ID_PM_10 for UNIT_GPM3 */
        return -1;
    case SAUL_SENSE_ID_CAPACITANCE:
        return -1;
    case SAUL_SENSE_ID_VOLTAGE:
        switch (data->unit) {
        case UNIT_V:
            switch (data->scale) {
            case -6:
                saul->obj_id = BTHOME_ID_VOLTAGE_FACTOR_0_001;
                saul->add_measurement = &_add_div_1000_uint16_measurement;
                return 0;
            case -3:
                saul->obj_id = BTHOME_ID_VOLTAGE_FACTOR_0_001;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            case -1:
                saul->obj_id = BTHOME_ID_VOLTAGE_FACTOR_0_1;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_PH:
        return -1;
    case SAUL_SENSE_ID_POWER:
        switch (data->unit) {
        case UNIT_W:
            switch (data->scale) {
            case -6:
                saul->obj_id = BTHOME_ID_POWER_UINT_FACTOR_0_01;
                saul->add_measurement = &_add_div_10000_uint16_measurement;
                return 0;
            case -4:
                saul->obj_id = BTHOME_ID_POWER_UINT_FACTOR_0_01;
                saul->add_measurement = &_add_div_100_uint16_measurement;
                return 0;
            case -2:
                saul->obj_id = BTHOME_ID_POWER_UINT_FACTOR_0_01;
                saul->add_measurement = &_add_uint16_measurement;
                return 0;
            default:
                return -1;
            }
        default:
            return -1;
        }
        return -1;
    case SAUL_SENSE_ID_SIZE:
        return -1;
    default:
        return -1;
    }
}

static void _reset_hdr(skald_bthome_ctx_t *ctx)
{
    /* Skip AD type, BTHome UUID, and BTHome Device Information */
    uint8_t *data_start = ctx->svc_data_len + 5U;
    ctx->skald.pkt.len = data_start - &ctx->skald.pkt.pdu[0];
    *ctx->svc_data_len = 4U;  /* AD type + BTHome UUID + BTHome Device Information */
}

static void _update_saul_measurements(skald_ctx_t *skald_ctx)
{
    skald_bthome_ctx_t *ctx = container_of(skald_ctx, skald_bthome_ctx_t, skald);
    uint8_t dev_idx = 0;
    uint8_t orig_last_dev_sent = ctx->last_dev_sent;

    _reset_hdr(ctx);
    skald_bthome_saul_t *ptr = ctx->devs;
    while (ptr) {
        if ((ctx->last_dev_sent == 0) ||
            (dev_idx > ctx->last_dev_sent)) {
            int dim = 1;
            int res = 0;
            phydat_t data = { 0 };

            if (ptr->saul.driver) {
                dim = saul_reg_read(&ptr->saul, &data);
                if (dim <= 0) {
                    continue;
                }
            }
            for (uint8_t i = 0; i < dim; i++) {
                if ((res = ptr->add_measurement(ctx, ptr->obj_id, &data, i)) < 0) {
                    break;
                }
            }
            if ((res == -EMSGSIZE) && (dev_idx > 0)) {
                ctx->last_dev_sent = dev_idx - 1;
            }
            if (res < 0) {
                break;
            }
        }
        ptr = container_of(
            ptr->saul.next, skald_bthome_saul_t, saul
        );
        dev_idx++;
    }
#if IS_USED(MODULE_SKALD_BTHOME_ENCRYPT)
    skald_bthome_encrypt(ctx);
#endif
    if ((ptr == NULL) ||
        /* or value just too big */
        (orig_last_dev_sent == ctx->last_dev_sent)) {
        /* reset device train */
        ctx->last_dev_sent = 0;
    }
}

int skald_bthome_saul_add(skald_bthome_ctx_t *ctx, skald_bthome_saul_t *saul)
{
    if ((saul->saul.driver != NULL) &&
        (saul->saul.driver->type & SAUL_CAT_MASK) != SAUL_CAT_SENSE) {
        return -ENOTSUP;
    }

    if (!ctx->skald.update_pkt) {
        ctx->skald.update_pkt = &_update_saul_measurements;
    }
    if (!(saul->flags & SKALD_BTHOME_SAUL_FLAGS_CUSTOM)) {
        phydat_t sample;
        int dim = saul_reg_read(&saul->saul, &sample);

        if (dim <= 0) {
            return -ENODEV;
        }
        if (_saul_sense_to_bthome_id(saul, &sample) < 0) {
            return -ENOENT;
        }
    }
    if (ctx->devs) {
        skald_bthome_saul_t *ptr = ctx->devs;

        if (ptr->obj_id > saul->obj_id) {
            saul->saul.next = &ptr->saul;
            ctx->devs = saul;
            return 0;
        }
        while (ptr) {
            skald_bthome_saul_t *next = container_of(
                ptr->saul.next, skald_bthome_saul_t, saul
            );
            /* insert in obj_id order */
            if (next) {
                if (next->obj_id > saul->obj_id) {
                    saul->saul.next = &next->saul;
                    ptr->saul.next = &saul->saul;
                    break;
                }
            }
            else {
                ptr->saul.next = &saul->saul;
                break;
            }
            ptr = next;
        }
    }
    else {
        ctx->devs = saul;
    }
    return 0;
}

static int _add_50perc_to_binary_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint8_measurement(ctx, obj_id, (data->val[idx] < 50) ? 0x00 : 0x01);
}

static int _add_button_press(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint8_measurement(ctx, obj_id, (data->val[idx]) ? 0x01 : 0x00);
}

static int _add_int8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_int8_measurement(ctx, obj_id, (int8_t)data->val[idx]);
}

static int _add_uint8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint8_measurement(ctx, obj_id, (uint8_t)data->val[idx]);
}

static int _add_int16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_int16_measurement(ctx, obj_id, data->val[idx]);
}

static int _add_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)data->val[idx]);
}

static int _add_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint24_measurement(ctx, obj_id, (uint32_t)data->val[idx]);
}

static int _add_g_force_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    int32_t d = data->val[idx];
    /* 1g ~= 9.807 m/s² */
    d *= 98;
    d /= 10;
    if (d > INT16_MAX) {
        d = INT16_MAX;
    }
    if (d < INT16_MIN) {
        d = INT16_MIN;
    }
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)d);
}

static int _add_ppb_to_ugm3_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    int32_t d = data->val[idx];
    /* see https://www.arcskoru.com/sites/default/files/Arc%20Guide%20to%20Re-Entry.pdf page 26 */
    d *= 3767;
    d /= 1000;
    if (d > INT16_MAX) {
        d = INT16_MAX;
    }
    if (d < INT16_MIN) {
        d = INT16_MIN;
    }
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)d);
}

static int _add_div_10_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] / 10));
}

static int _add_div_10_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint24_measurement(ctx, obj_id, (uint32_t)(data->val[idx] / 10));
}

static int _add_div_100_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] / 100));
}

static int _add_div_100_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint24_measurement(ctx, obj_id, (uint32_t)(data->val[idx] / 100));
}

static int _add_div_1000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] / 1000));
}

static int _add_div_10000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] / 10000));
}

static int _add_times_10_uint8_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint8_measurement(ctx, obj_id, (uint8_t)(data->val[idx] * 10));
}

static int _add_times_10_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] * 10));
}

static int _add_times_100_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] * 100));
}

static int _add_times_10_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint24_measurement(ctx, obj_id, (uint32_t)(data->val[idx] * 10));
}

static int _add_times_100_uint24_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint24_measurement(ctx, obj_id, (uint32_t)(data->val[idx] * 100));
}

static int _add_times_1000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] * 1000));
}

static int _add_times_10000_uint16_measurement(
                skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    return skald_bthome_add_uint16_measurement(ctx, obj_id, (uint16_t)(data->val[idx] * 1000));
}

/** @} */
