/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <string.h>

#include "net/ieee802154.h"
#include "net/zep.h"
#include "zep_parser.h"

#define SOCKET_ZEP_V2_TYPE_HELLO   (255)

bool zep_parse_mac(const void *buffer, size_t len, void *out, uint8_t *out_len)
{
    const void *payload;
    const zep_v2_data_hdr_t *zep = buffer;

    if (len == 0) {
        return false;
    }

    if ((zep->hdr.preamble[0] != 'E') || (zep->hdr.preamble[1] != 'X')) {
        return false;
    }

    if (zep->hdr.version != 2) {
        return false;
    }

    switch (zep->type) {
    case ZEP_V2_TYPE_DATA:
        payload = (zep_v2_data_hdr_t *)zep + 1;
        break;
    case ZEP_V2_TYPE_ACK:
        payload = (zep_v2_ack_hdr_t *)zep + 1;
        break;
    case SOCKET_ZEP_V2_TYPE_HELLO:
        /* HELLO packet only contains HW addr as payload */
        payload = (zep_v2_data_hdr_t *)zep + 1;
        *out_len = zep->length;

        if (*out_len < zep->length) {
            return false;
        }

        memcpy(out, payload, zep->length);
        *out_len = zep->length;
        return true;
    default:
        return false;
    }

    le_uint16_t dst_pan;
    int res = ieee802154_get_src(payload, out, &dst_pan);

    if (res <= 0) {
        return false;
    }

    *out_len = res;

    /* check that we are not out of bounds */
    return (uintptr_t)payload + *out_len < (uintptr_t)buffer + len;
}

void zep_set_lqi(void *buffer, uint8_t lqi)
{
    zep_v2_data_hdr_t *zep = buffer;

    if (zep->type != ZEP_V2_TYPE_DATA) {
        return;
    }

    zep->lqi_val = lqi;
}
