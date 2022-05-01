/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_ps
 * @{
 * @file
 * @brief   IKE
 * @author      Juliusz Neuman <neuman.juliusz@gmail.com>
 * @}
 */

#include "ike/chunk.h"
#include <stdio.h>
#include <stdlib.h>
#include "od.h"

chunk_t empty_chunk = {
    .ptr = NULL,
    .len = 0,
};
void free_chunk(chunk_t *chunk)
{
    if (chunk->len) {
        free(chunk->ptr);
    }
    chunk->len = 0;
}

chunk_t malloc_chunk(size_t size)
{
    chunk_t chunk = {
        .len = size,
        .ptr = malloc(size),
    };

    return chunk;
}

void printf_chunk(chunk_t chunk, uint8_t width)
{
    od_hex_dump(chunk.ptr, chunk.len, width);
}
