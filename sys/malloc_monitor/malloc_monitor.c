/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   monitor heap memory usage (calls to malloc/realloc/free)
 * @author  Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#include <stdio.h>
#include <string.h>

#include "architecture.h"
#include "assert.h"
#include "cpu.h"
#include "irq.h"
#include "mutex.h"

#include "malloc_monitor.h"
#include "malloc_monitor_internal.h"

#ifndef CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE
#define CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE 100
#endif

#ifndef CONFIG_MODULE_SYS_MALLOC_MONITOR_VERBOSE
#define CONFIG_MODULE_SYS_MALLOC_MONITOR_VERBOSE 0
#endif

static struct {
    void *addr[CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE];
    size_t size[CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE];
    size_t current;
    size_t high_watermark;
} malloc_monitor = {
    .addr = {NULL},
    .current = 0,
    .high_watermark = 0,
};

/* guards access to malloc_monitor */
static mutex_t _lock;

void malloc_monitor_add(void *ptr, size_t size, uinttxtptr_t pc, char *func_prefix)
{
    if (ptr == NULL) {
        return;
    }
#if CONFIG_MODULE_SYS_MALLOC_MONITOR_VERBOSE
    printf("%salloc(%" PRIuSIZE ") @ 0x%" PRIxTXTPTR " returned %p\n",
        func_prefix, size, pc, ptr);
#endif
    assert(!irq_is_in());
    mutex_lock(&_lock);
    for (uint8_t i=0; i<CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE; i++) {
        if (malloc_monitor.addr[i] == NULL) {
            malloc_monitor.addr[i] = ptr;
            malloc_monitor.size[i] = size;
            malloc_monitor.current += size;
            if (malloc_monitor.current > malloc_monitor.high_watermark) {
                malloc_monitor.high_watermark = malloc_monitor.current;
            }
            mutex_unlock(&_lock);
            return;
        }
    }
    mutex_unlock(&_lock);
    printf("malloc_monitor: maximum number of pointers to be monitored "
        "(as set by CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE) exceeded.\n");
    (void)func_prefix;
    (void)pc;
}

void malloc_monitor_rm(void *ptr, uinttxtptr_t pc)
{
    if (ptr == NULL) {
        return;
    }
#if CONFIG_MODULE_SYS_MALLOC_MONITOR_VERBOSE
    printf("malloc_monitor: free(%p) @ 0x%" PRIxTXTPTR " \n", ptr, pc);
#endif
    assert(!irq_is_in());
    mutex_lock(&_lock);
    for (uint8_t i=0; i<CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE; i++) {
        if (malloc_monitor.addr[i] == ptr) {
            malloc_monitor.addr[i] = NULL;
            malloc_monitor.current -= malloc_monitor.size[i];
            mutex_unlock(&_lock);
            return;
        }
    }
    mutex_unlock(&_lock);
    printf("malloc_monitor: free(%p) @ 0x%" PRIxTXTPTR " invalid\n", ptr, pc);
}

void malloc_monitor_mv(void *ptr_old, void *ptr_new, size_t size_new, uinttxtptr_t pc)
{
    if (ptr_old == NULL) {
        malloc_monitor_add(ptr_new, size_new, pc, "re");
        return;
    }
    if (size_new == 0) {
        malloc_monitor_rm(ptr_old, pc);
        return;
    }
    if (ptr_new == NULL) {
        return;
    }
#if CONFIG_MODULE_SYS_MALLOC_MONITOR_VERBOSE
    printf("malloc_monitor: realloc(%p, %" PRIuSIZE ") @0x%" PRIxTXTPTR " returned %p\n",
        ptr_old, size_new, pc, ptr_new);
#endif
    assert(!irq_is_in());
    mutex_lock(&_lock);
    for (uint8_t i=0; i<CONFIG_MODULE_SYS_MALLOC_MONITOR_SIZE; i++) {
        if (malloc_monitor.addr[i] == ptr_old) {
            malloc_monitor.addr[i] = ptr_new;
            size_t size_old = malloc_monitor.size[i];
            malloc_monitor.size[i] = size_new;
            if (size_new > size_old) {
                malloc_monitor.current += size_new - size_old;
                if (malloc_monitor.current > malloc_monitor.high_watermark) {
                    malloc_monitor.high_watermark = malloc_monitor.current;
                }
            }
            else {
                malloc_monitor.current -= size_old - size_new;
            }
            mutex_unlock(&_lock);
            return;
        }
    }
    mutex_unlock(&_lock);
    printf("malloc_monitor: realloc(%p) @ 0x%" PRIxTXTPTR " invalid\n", ptr_old, pc);
}

size_t malloc_monitor_get_usage_current(void)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    size_t ret = malloc_monitor.current;
    mutex_unlock(&_lock);
    return ret;
}

size_t malloc_monitor_get_usage_high_watermark(void)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    size_t ret = malloc_monitor.high_watermark;
    mutex_unlock(&_lock);
    return ret;
}

void malloc_monitor_reset_high_watermark(void)
{
    assert(!irq_is_in());
    mutex_lock(&_lock);
    malloc_monitor.high_watermark = malloc_monitor.current;
    mutex_unlock(&_lock);
}

/** @} */
