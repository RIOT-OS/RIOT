/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *               2020 Koen Zandberg <koen@bergzand.net>
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

    memarray_extend(mem, data, num);
}

void memarray_extend(memarray_t *mem, void *data, size_t num)
{
    for (uint8_t *element = data;
         element < (uint8_t*)data + (num * mem->size);
         element += mem->size) {
        memarray_free(mem, element);
    }
}

static bool _in_pool(const memarray_t *mem, const void *data, size_t num,
                     const void *element)
{
    return element >= data &&
        (uint8_t*)element < ((uint8_t*)data + (mem->size * num));
}

int memarray_reduce(memarray_t *mem, void *data, size_t num)
{
    /* Number of free chunks found inside the pool to be freed */
    size_t remaining = num;

    /* Keep a clist around of found elements in case we need to restore */
    memarray_element_t *found_elements = NULL;
    /* Cast it to memarray_element_t, makes things easier to read */
    memarray_element_t **element_ptr  = (memarray_element_t**)&mem->free_data;

    while (*element_ptr) {
        DEBUG("memarray: At element %p -> %p\n",
              (void*)*element_ptr, (void*)(*element_ptr)->next);
        if (_in_pool(mem, data, num, *element_ptr)) {

            /* Save the element */
            memarray_element_t *found_element = *element_ptr;
            DEBUG("memarray: Found %p in %p, at %u\n",
                  (void*)found_element, data, (unsigned)remaining);

            /* Copy pointer over to previous element remove it from the pool
             * free list */
            memcpy(element_ptr, (*element_ptr), sizeof(void*));

            if (found_elements) {
                found_element->next = found_elements->next;
                found_elements->next = found_element;
            }
            else {
                found_element->next = found_element;
            }
            found_elements = found_element;

            remaining--;
            if (remaining == 0) {
                /* Early return if all elements are removed */
                return 0;
            }
        }
        else {
            element_ptr = &(*element_ptr)->next;
        }
    }

    /* Not all elements found in free list, re-add them to the memarray */
    DEBUG("memarray: Missing elements, restoring");
    if (found_elements) {
        void *swap = mem->free_data;
        mem->free_data = found_elements->next;
        found_elements->next = swap;
    }

    return -1;
}

size_t memarray_available(memarray_t *mem)
{
    size_t num = 0;
    void **element = &mem->free_data;
    while (*element) {
        element = (void**)*element;
        num++;
    }
    return num;
}
