/*
 * Copyright (C) 2019 Gunar Schorcht
 *               2022 Otto-von-Guericke-Universität Magdeburg
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
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdio.h>
#include <string.h>

#include "architecture.h"
#include "assert.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_defines.h"
#include "malloc_monitor_internal.h"
#include "mutex.h"

extern void *__real_malloc(size_t size);
extern void __real_free(void *ptr);
extern void *__real_realloc(void *ptr, size_t size);

static mutex_t _lock;

void __attribute__((used)) *__wrap_malloc(size_t size)
{
    uinttxtptr_t pc;
    if (IS_USED(MODULE_MALLOC_TRACING)) {
        pc = cpu_get_caller_pc();
    }
    assert(!irq_is_in());
    mutex_lock(&_lock);
    void *ptr = __real_malloc(size);
    if (IS_USED(MODULE_MALLOC_MONITOR)) {
        malloc_monitor_add(ptr, size, cpu_get_caller_pc(), "m");
    }
    mutex_unlock(&_lock);
    if (IS_USED(MODULE_MALLOC_TRACING)) {
        printf("malloc(%" PRIuSIZE ") @ 0x%" PRIxTXTPTR " returned %p\n",
               size, pc, ptr);
    }
    return ptr;
}

void __attribute__((used)) __wrap_free(void *ptr)
{
    if (IS_USED(MODULE_MALLOC_TRACING)) {
        uinttxtptr_t pc = cpu_get_caller_pc();
        printf("free(%p) @ 0x%" PRIxTXTPTR ")\n", ptr, pc);
    }
    assert(!irq_is_in());
    mutex_lock(&_lock);
    __real_free(ptr);
    if (IS_USED(MODULE_MALLOC_MONITOR)) {
        malloc_monitor_rm(ptr, cpu_get_caller_pc());
    }
    mutex_unlock(&_lock);
}

void * __attribute__((used)) __wrap_calloc(size_t nmemb, size_t size)
{
    uinttxtptr_t pc;
    if (IS_USED(MODULE_MALLOC_TRACING)) {
        pc = cpu_get_caller_pc();
    }
    /* some c libs don't perform proper overflow check (e.g. newlib < 4.0.0). Hence, we
     * just implement calloc on top of malloc ourselves. In addition to ensuring proper
     * overflow checks, this likely saves a bit of ROM */
    size_t total_size;
    if (__builtin_mul_overflow(nmemb, size, &total_size)) {
        if (IS_USED(MODULE_MALLOC_TRACING)) {
            printf("calloc(%" PRIuSIZE ", %" PRIuSIZE ") @ 0x%" PRIxTXTPTR " overflowed\n",
                   nmemb, size, pc);
        }
        return NULL;
    }

    mutex_lock(&_lock);
    void *res = __real_malloc(total_size);
    if (IS_USED(MODULE_MALLOC_MONITOR)) {
        malloc_monitor_add(res, total_size, cpu_get_caller_pc(), "c");
    }
    mutex_unlock(&_lock);
    if (res) {
        memset(res, 0, total_size);
    }

    if (IS_USED(MODULE_MALLOC_TRACING)) {
        printf("calloc(%" PRIuSIZE ", %" PRIuSIZE ") @ 0x%" PRIxTXTPTR " returned %p\n",
               nmemb, size, pc, res);
    }

    return res;
}

void * __attribute__((used))__wrap_realloc(void *ptr, size_t size)
{
    uinttxtptr_t pc;
    if (IS_USED(MODULE_MALLOC_TRACING)) {
        pc = cpu_get_caller_pc();
    }

    assert(!irq_is_in());
    mutex_lock(&_lock);
    void *new = __real_realloc(ptr, size);
    if (IS_USED(MODULE_MALLOC_MONITOR)) {
        malloc_monitor_mv(ptr, new, size, cpu_get_caller_pc());
    }
    mutex_unlock(&_lock);

    if (IS_USED(MODULE_MALLOC_TRACING)) {
        printf("realloc(%p, %" PRIuSIZE ") @0x%" PRIxTXTPTR " returned %p\n",
               ptr, size, pc, new);
    }
    return new;
}

/** @} */
