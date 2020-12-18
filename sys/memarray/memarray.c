/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <string.h>
#include "memarray.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void memarray_init(memarray_t *mem, void *data, size_t size, size_t num)
{
    assert((mem != NULL) && (data != NULL) && (size >= sizeof(void *)) &&
           (num != 0));

    DEBUG("memarray: Initialize memarray of %u times %u Bytes at %p\n",
          (unsigned)num, (unsigned)size, data);

    mem->free_data = NULL;
    mem->size = size;
    mem->num = num;

    for (uint8_t *element = data;
         element < (uint8_t*)data + (num * size);
         element += size) {
        memarray_free(mem, element);
    }
}
