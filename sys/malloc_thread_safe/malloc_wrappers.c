/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Implements various POSIX syscalls
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#include "assert.h"
#include "irq.h"
#include "mutex.h"

extern void *__real_malloc(size_t size);
extern void __real_free(void *ptr);
extern void *__real_calloc(size_t nmemb, size_t size);
extern void *__real_realloc(void *ptr, size_t size);

static mutex_t _lock;

void *__wrap_malloc(size_t size)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    void *ptr = __real_malloc(size);
    mutex_unlock(&_lock);
    return ptr;
}

void __wrap_free(void *ptr)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    __real_free(ptr);
    mutex_unlock(&_lock);
}

void *__wrap_calloc(size_t nmemb, size_t size)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    void *ptr = __real_calloc(nmemb, size);
    mutex_unlock(&_lock);
    return ptr;
}

void *__wrap_realloc(void *ptr, size_t size)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    void *new = __real_realloc(ptr, size);
    mutex_unlock(&_lock);
    return new;
}

/** @} */
