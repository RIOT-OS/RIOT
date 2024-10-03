/*
 * Copyright (C) 2018,2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     ble_bluetil_ad
 * @{
 *
 * @file
 * @brief       Implementation of the generic BLE advertising data processing
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "net/bluetil/ad.h"

#define POS_TYPE            (1U)
#define POS_DATA            (2U)

void bluetil_ad_init(bluetil_ad_t *ad, void *buf, size_t pos,  size_t size)
{
    assert(ad);
    assert(buf);

    ad->buf = buf;
    ad->pos = pos;
    ad->size = size;
}

int bluetil_ad_find(const bluetil_ad_t *ad, uint8_t type,
                    bluetil_ad_data_t *data)
{
    assert(ad);
    assert(data);

    unsigned pos = 0;

    while ((pos + POS_DATA) < ad->pos) {
        uint8_t len = ad->buf[pos];

        if (pos + len >= ad->pos) {
            return BLUETIL_AD_NOMEM;
        }

        if (ad->buf[pos + POS_TYPE] == type) {
            data->data = ad->buf + pos + POS_DATA;
            data->len = len - 1;           /* take away the type field */
            return BLUETIL_AD_OK;
        }

        pos += (len + 1);
    }

    return BLUETIL_AD_NOTFOUND;
}

int bluetil_ad_find_and_cmp(const bluetil_ad_t *ad, uint8_t type,
                            const void *val, size_t val_len)
{
    bluetil_ad_data_t field;

    if (bluetil_ad_find(ad, type, &field) == BLUETIL_AD_OK) {
        return ((field.len == val_len) && memcmp(val, field.data, val_len) == 0);
    }
    return 0;
}

int bluetil_ad_find_str(const bluetil_ad_t *ad, uint8_t type,
                        char *str, size_t str_len)
{
    bluetil_ad_data_t f;
    int res = bluetil_ad_find(ad, type, &f);
    if (res != BLUETIL_AD_OK) {
        return res;
    }

    size_t len = (f.len >= str_len) ? (str_len - 1) : f.len;
    memcpy(str, f.data, len);
    str[len] = '\0';

    return BLUETIL_AD_OK;
}

int bluetil_ad_add(bluetil_ad_t *ad, uint8_t field_type,
                   const void *data, size_t data_len)
{
    assert(ad);

    if ((ad->pos + 2 + data_len) > ad->size) {
        return BLUETIL_AD_NOMEM;
    }
    ad->buf[ad->pos++] = data_len + 1;
    ad->buf[ad->pos++] = field_type;
    memcpy(&ad->buf[ad->pos], data, data_len);
    ad->pos += data_len;

    return BLUETIL_AD_OK;
}
