/*
 * SPDX-FileCopyrightText: 2015 Nico von Geyso <nico.geyso@fu-berlin.de>
 * SPDX-FileCopyrightText: 2015 René Kijewski <rene.kijewski@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "crypto/helper.h"

void crypto_block_inc_ctr(uint8_t block[16], size_t ctr_len)
{
    uint8_t *b = &block[15];

    for (size_t i = 0; i < ctr_len; ++i, --b) {
        if (++*b != 0) {
            break;
        }
    }
}

bool crypto_equals(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0;

    for (size_t i = 0; i < len; ++i, ++a, ++b) {
        diff |= (*a ^ *b);
    }

    diff |= (diff >> 1) | (diff << 7);
    diff |= (diff >> 2) | (diff << 6);
    diff |= (diff >> 4) | (diff << 4);
    ++diff;

    return (bool)diff;
}

/* Compiler should not be allowed to optimize this */
void crypto_secure_wipe(void *buf, size_t len)
{
    volatile uint8_t *vbuf = buf;

    for (size_t i = 0; i < len; i++) {
        vbuf[i] = 0;
    }
}
