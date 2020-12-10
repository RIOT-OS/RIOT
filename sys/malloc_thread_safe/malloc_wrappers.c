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

#include "irq.h"

extern void *__real_malloc(size_t size);
extern void __real_free(void *ptr);
extern void *__real_calloc(size_t nmemb, size_t size);
extern void *__real_realloc(void *ptr, size_t size);

void *__wrap_malloc(size_t size)
{
    unsigned state = irq_disable();
    void *ptr = __real_malloc(size);
    irq_restore(state);
    return ptr;
}

void __wrap_free(void *ptr)
{
    unsigned state = irq_disable();
    __real_free(ptr);
    irq_restore(state);
}

void *__wrap_calloc(size_t nmemb, size_t size)
{
    unsigned state = irq_disable();
    void *ptr = __real_calloc(nmemb, size);
    irq_restore(state);
    return ptr;
}

void *__wrap_realloc(void *ptr, size_t size)
{
    unsigned state = irq_disable();
    void *new = __real_realloc(ptr, size);
    irq_restore(state);
    return new;
}

/** @} */
