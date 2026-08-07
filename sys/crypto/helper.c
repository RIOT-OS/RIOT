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

bool crypto_equals(volatile const uint8_t *a, volatile const uint8_t *b,
                   size_t len)
{
    uint8_t diff = 0;

    for (size_t i = 0; i < len; ++i, ++a, ++b) {
        diff |= (*a ^ *b);
    }

    /* If there was at least one bit different between `a` and `b`, at least
     * one bit is set in `diff` now. If all bits in `a` and `b` where equal,
     * all bits in `diff` are clear. */

    diff |= (diff >> 1) | (diff << 7);
    diff |= (diff >> 2) | (diff << 6);
    diff |= (diff >> 4) | (diff << 4);

    /* If previously at least one bit in `diff` was set (so at least one bit in
     * `a` and `b` differ), now all bits are set. In that case incrementing will
     * overflow it to `0`.
     *
     * If none of the bits were set (so `a` and `b` were bitwise identical),
     * still none of the bits are set. In that case incrementing will give us
     * `1`. */
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
