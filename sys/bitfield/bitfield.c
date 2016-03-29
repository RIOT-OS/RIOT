/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_util
 * @{
 *
 * @file
 * @brief       Bitfield auxillary functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include "bitfield.h"
#include "irq.h"

int bf_get_unset(uint8_t field[], int size)
{
    int result = -1;
    int nbytes = (size + 7) / 8;
    int i = 0;

    unsigned state = irq_disable();

    /* skip full bytes */
    for (int j = 0; (j < nbytes) && (field[j] == 255); j++) {
        i += 8;
    }

    for (; i < size; i++) {
        if (!bf_isset(field, i)) {
            bf_set(field, i);
            result = i;
            break;
        }
    }

    irq_restore(state);
    return(result);
}
