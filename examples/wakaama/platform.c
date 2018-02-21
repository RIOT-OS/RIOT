/*******************************************************************************
*
* Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Christian Manal - Ported to RIOT OS
*******************************************************************************/

#include <liblwm2m.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "lwm2mconfig.h"
#include "memarray_limits.h"

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

/*
 * Raise statistics about memory allocation.
 * (Uncomment to enable).
 */
#if ENABLE_DEBUG == 1
/* #define DEBUG_MALLOC */
#endif /* ENABLE_DEBUG */

#ifndef LWM2M_MEMORY_TRACE

#ifdef DEBUG_MALLOC
#define max(a, b) ((a) > (b)) ? (a) : (b)
uint16_t cnt16 = 0;
uint16_t cnt16_max = 0;
uint16_t cnt32 = 0;
uint16_t cnt32_max = 0;
uint16_t cnt64 = 0;
uint16_t cnt64_max = 0;
uint16_t cnt192 = 0;
uint16_t cnt192_max = 0;
uint16_t cnt512 = 0;
uint16_t cnt512_max = 0;
#endif /* DEBUG_MALLOC */

uint32_t b16_data[4][MAX_16BYTE_BLOCKS];
uint32_t b32_data[8][MAX_32BYTE_BLOCKS];
uint32_t b64_data[16][MAX_64BYTE_BLOCKS];
uint32_t b192_data[48][MAX_192BYTE_BLOCKS];
uint32_t b512_data[128][MAX_512BYTE_BLOCKS];
memarray_t b16_storage;
memarray_t b32_storage;
memarray_t b64_storage;
memarray_t b192_storage;
memarray_t b512_storage;

void lwm2m_memarray_print_stats(void)
{
#ifdef DEBUG_MALLOC
    DEBUG("cnt16: %"PRIu16"/%u max: %"PRIu16"\n", cnt16, MAX_16BYTE_BLOCKS, cnt16_max);
    DEBUG("cnt32: %"PRIu16"/%u max: %"PRIu16"\n", cnt32, MAX_32BYTE_BLOCKS, cnt32_max);
    DEBUG("cnt64: %"PRIu16"/%u max: %"PRIu16"\n", cnt64, MAX_64BYTE_BLOCKS, cnt64_max);
    DEBUG("cnt192: %"PRIu16"/%u max: %"PRIu16"\n", cnt192, MAX_192BYTE_BLOCKS, cnt192_max);
    DEBUG("cnt512: %"PRIu16"/%u max: %"PRIu16"\n", cnt512, MAX_512BYTE_BLOCKS, cnt512_max);
#endif /* DEBUG_MALLOC */
    return;
}

void lwm2m_memarray_init(void)
{
    memarray_init(&b16_storage, b16_data, 16, MAX_16BYTE_BLOCKS);
    memarray_init(&b32_storage, b32_data, 32, MAX_32BYTE_BLOCKS);
    memarray_init(&b64_storage, b64_data, 64, MAX_64BYTE_BLOCKS);
    memarray_init(&b192_storage, b192_data, 192, MAX_192BYTE_BLOCKS);
    memarray_init(&b512_storage, b512_data, 512, MAX_512BYTE_BLOCKS);
}

void *lwm2m_malloc(size_t s)
{
    if (s <= 16) {
#ifdef DEBUG_MALLOC
        cnt16++;
        cnt16_max = max(cnt16, cnt16_max);
#endif  /* DEBUG_MALLOC */
        return memarray_alloc(&b16_storage);
    }
    else if (s <= 32) {
#ifdef DEBUG_MALLOC
        cnt32++;
        cnt32_max = max(cnt32, cnt32_max);
#endif  /* DEBUG_MALLOC */
        return memarray_alloc(&b32_storage);
    }
    else if (s <= 64) {
#ifdef DEBUG_MALLOC
        cnt64++;
        cnt64_max = max(cnt64, cnt64_max);
#endif  /* DEBUG_MALLOC */
        return memarray_alloc(&b64_storage);
    }
    else if (s <= 192) {
#ifdef DEBUG_MALLOC
        cnt192++;
        cnt192_max = max(cnt192, cnt192_max);
#endif  /* DEBUG_MALLOC */
        return memarray_alloc(&b192_storage);
    }
    else if (s <= 512) {
#ifdef DEBUG_MALLOC
        cnt512++;
        cnt512_max = max(cnt512, cnt512_max);
#endif  /* DEBUG_MALLOC */
        return memarray_alloc(&b512_storage);
    }
    DEBUG("lwm2m_malloc(): Requested %u bytes of memory but got no pool for this size.\n", s);
    return NULL;
}

void lwm2m_free(void *p)
{
    uintptr_t start = (uintptr_t)b16_data;
    uintptr_t end   = start + (uintptr_t)sizeof(b16_data);
    uintptr_t pi = (uintptr_t)p;

    if (pi >= start && pi < end) {
#ifdef DEBUG_MALLOC
        cnt16--;
#endif  /* DEBUG_MALLOC */
        memarray_free(&b16_storage, p);
        return;
    }

    start = (uintptr_t)b32_data;
    end   = start + (uintptr_t)sizeof(b32_data);
    if (pi >= start && pi < end) {
#ifdef DEBUG_MALLOC
        cnt32--;
#endif  /* DEBUG_MALLOC */
        memarray_free(&b32_storage, p);
        return;
    }

    start = (uintptr_t)b64_data;
    end   = start + (uintptr_t)sizeof(b64_data);
    if (pi >= start && pi < end) {
#ifdef DEBUG_MALLOC
        cnt64--;
#endif  /* DEBUG_MALLOC */
        memarray_free(&b64_storage, p);
        return;
    }

    start = (uintptr_t)b192_data;
    end   = start + (uintptr_t)sizeof(b192_data);
    if (pi >= start && pi < end) {
#ifdef DEBUG_MALLOC
        cnt192--;
#endif  /* DEBUG_MALLOC */
        memarray_free(&b192_storage, p);
        return;
    }

    start = (uintptr_t)b512_data;
    end   = start + (uintptr_t)sizeof(b512_data);
    if (pi >= start && pi < end) {
#ifdef DEBUG_MALLOC
        cnt512--;
#endif  /* DEBUG_MALLOC */
        memarray_free(&b512_storage, p);
        return;
    }

    DEBUG("lwm2m_free(): Tried to free memory block that doesn't belong to a known pool\n");
    return;
}

char *lwm2m_strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    void *new = lwm2m_malloc(len);

    if (new == NULL) {
        return NULL;
    }

    return (char *)memcpy(new, str, len);
}

#endif /* LWM2M_MEMORY_TRACE */

int lwm2m_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

time_t lwm2m_gettime(void)
{
    struct timeval tv;

    if (0 != gettimeofday(&tv, NULL)) {
        return -1;
    }

    return tv.tv_sec;
}

void lwm2m_printf(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);

    vfprintf(stderr, format, ap);

    va_end(ap);
}
