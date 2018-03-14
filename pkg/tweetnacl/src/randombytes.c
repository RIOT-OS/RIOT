/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>

#include "random.h"

#include <stdio.h>

void randombytes(uint8_t *target, uint64_t n)
{
    /* tweetnacl needs uint64_t as "n" parameter, random provides uint32 */
    random_bytes(target, n);
}
