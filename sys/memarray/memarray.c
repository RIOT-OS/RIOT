/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include "memarray.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void memarray_init(memarray_t *mem, void *data, size_t size, size_t num)
{
    assert((mem != NULL) && (data != NULL) && (size >= sizeof(void *)) &&
           (num != 0));

    DEBUG("memarray: Initialize memarray of %u times %u Bytes at %p\n",
          (unsigned)num, (unsigned)size, data);

    mem->free_data = data;
    mem->size = size;
    mem->num = num;

    for (size_t i = 0; i < (mem->num - 1); i++) {
        void *next = ((char *)mem->free_data) + ((i + 1) * mem->size);
        memcpy(((char *)mem->free_data) + (i * mem->size), &next, sizeof(void *));
    }
}

void *memarray_alloc(memarray_t *mem)
{
    assert(mem != NULL);

    if (mem->free_data == NULL) {
        return NULL;
    }
    void *free = mem->free_data;
    mem->free_data = *((void **)mem->free_data);
    DEBUG("memarray: Allocate %u Bytes at %p\n", (unsigned)mem->size, free);
    return free;
}

void memarray_free(memarray_t *mem, void *ptr)
{
    assert((mem != NULL) && (ptr != NULL));

    memcpy(ptr, &mem->free_data, sizeof(void *));
    mem->free_data = ptr;
    DEBUG("memarray: Free %u Bytes at %p\n", (unsigned)mem->size, ptr);
}
