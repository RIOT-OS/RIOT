/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Bitfield auxiliary functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include "bitfield.h"
#include "irq.h"

static inline unsigned _skip_bytes(const uint8_t field[], unsigned nbytes, uint8_t byte)
{
    unsigned i = 0;
    for (unsigned j = 0; (j < nbytes) && (field[j] == byte); j++) {
        i += 8;
    }
    return i;
}

int bf_get_unset(uint8_t field[], size_t size)
{
    int result = -1;
    unsigned nbytes = (size + 7) / 8;

    unsigned state = irq_disable();

    /* skip full bytes */
    unsigned i = _skip_bytes(field, nbytes, 0xff);

    for (; i < size; i++) {
        if (!bf_isset(field, i)) {
            bf_set(field, i);
            result = i;
            break;
        }
    }

    irq_restore(state);
    return result;
}

int bf_find_first_set(const uint8_t field[], size_t size)
{
    unsigned nbytes = (size + 7) / 8;
    unsigned i = _skip_bytes(field, nbytes, 0);

    for (; i < size; i++) {
        if (bf_isset(field, i)) {
            return i;
        }
    }

    return -1;
}

int bf_find_first_unset(const uint8_t field[], size_t size)
{
    unsigned nbytes = (size + 7) / 8;
    unsigned i = _skip_bytes(field, nbytes, 0xff);

    for (; i < size; i++) {
        if (!bf_isset(field, i)) {
            return i;
        }
    }

    return -1;
}
