/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG    0
#define MEMLEAK_DEBUG   0
#include "debug.h"

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/reent.h>
#include <sys/stat.h>

#include <malloc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ets_sys.h"
#include "c_types.h"

#include "common.h"
#include "cpu_conf.h"
#include "irq.h"
#include "kernel_defines.h"
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "sched.h"
#include "syscalls.h"

#include "esp/xtensa_ops.h"
#include "esp/common_macros.h"

#include "sdk/sdk.h"

#ifdef MODULE_ESP_SDK
/**
 * Map memory management functions to SDK memory management functions.
 * This is necessary to use the same heap as the SDK internally does.
 * Furthermore, these functions at least avoid interrupts during the
 * execution of memory management functions. Memory management functions
 * of ETS are not used and have not to be considered therefore.
 */
extern void *pvPortMalloc (size_t size, const char *, unsigned);
extern void vPortFree (void *ptr, const char *, unsigned);
extern void *pvPortZalloc (size_t size, const char *, unsigned);
extern void *pvPortCalloc (size_t nmemb, size_t size, const char *, unsigned);
extern void *pvPortRealloc (void *ptr, size_t size, const char *, unsigned);
extern unsigned int xPortGetFreeHeapSize(void);

void *__real_malloc(size_t size);
void  __real_free(void *ptr);
void *__real_calloc(size_t nmemb, size_t size);
void *__real_realloc(void *ptr, size_t size);
void *__real__malloc_r (struct _reent *r, size_t size);
void  __real__free_r (struct _reent *r, void *ptr);
void *__real__realloc_r (struct _reent *r, void *ptr, size_t size);
struct mallinfo __real_mallinfo(void);

void* IRAM __wrap_malloc(size_t size)
{
    #if MEMLEAK_DEBUG
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
    return pvPortMalloc(size, mem_debug_file, __LINE__);
    #else
    return pvPortMalloc(size, "", 0);
    #endif
}

void IRAM __wrap_free(void *ptr)
{
    #if MEMLEAK_DEBUG
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
    return vPortFree (ptr, mem_debug_file, __LINE__);
    #else
    return vPortFree (ptr, "", 0);
    #endif
}

void* IRAM __wrap_calloc(size_t nmemb, size_t size)
{
    #if MEMLEAK_DEBUG
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
    return pvPortCalloc(nmemb, size, mem_debug_file, __LINE__);
    #else
    return pvPortCalloc(nmemb, size, "", 0);
    #endif
}

void* IRAM __wrap_realloc(void *ptr, size_t size)
{
    #if MEMLEAK_DEBUG
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
    return pvPortRealloc(ptr, size, mem_debug_file, __LINE__);
    #else
    return pvPortRealloc(ptr, size, "", 0);
    #endif
}

void* IRAM __wrap__malloc_r (struct _reent *r, size_t size)
{
    return __wrap_malloc (size);
}

void IRAM __wrap__free_r (struct _reent *r, void *ptr)
{
    __wrap_free (ptr);
}

void IRAM *__wrap__realloc_r (struct _reent *r, void *ptr, size_t size)
{
    return __wrap_realloc (ptr, size);
}

unsigned int get_free_heap_size (void)
{
    return xPortGetFreeHeapSize();
}

extern uint8_t  _eheap;     /* end of heap (defined in esp8266.riot-os.app.ld) */
extern uint8_t  _sheap;     /* start of heap (defined in esp8266.riot-os.app.ld) */

struct mallinfo __wrap_mallinfo(void)
{
    struct mallinfo mi;

    mi.arena = &_eheap - &_sheap;
    mi.fordblks = get_free_heap_size();
    mi.uordblks = mi.arena - mi.fordblks;
    mi.keepcost = mi.fordblks;
    return mi;
}

void heap_stats(void)
{
    struct mallinfo minfo = __wrap_mallinfo();
    ets_printf("heap: %d (used %d, free %d)\n",
               minfo.arena, minfo.uordblks, minfo.fordblks);
}

void IRAM syscalls_init (void)
{
}

#else   /* MODULE_ESP_SDK */
/*
 * To use the same heap as SDK internally does, SDK memory management
 * functions have to be replaced by newlib memory functions. In that case,
 * the _malloc_lock/_unlock functions have to be defined. Memory management
 * functions of ETS are not used and have not to be considered here.
 */

void* IRAM pvPortMalloc (size_t size, const char *file, unsigned line)
{
    (void)file;
    (void)line;

    return malloc (size);
}

void IRAM vPortFree (void *ptr, const char *file, unsigned line)
{
    (void)file;
    (void)line;

    free (ptr);
}

void* IRAM pvPortCalloc (size_t nmemb, size_t size, const char *file, unsigned line)
{
    (void)file;
    (void)line;

    void *ptr = malloc (nmemb*size);
    if (ptr) {
        memset (ptr, 0x0, nmemb*size);
    }
    return ptr;
}

void* IRAM pvPortZalloc (size_t size, const char *file, unsigned line)
{
    (void)file;
    (void)line;

    void *ptr = malloc (size);
    if (ptr) {
        memset (ptr, 0x0, size);
    }
    return ptr;
}

void* IRAM pvPortRealloc (void *ptr, size_t size, const char *file, unsigned line)
{
    (void)file;
    (void)line;

    return realloc (ptr, size);
}

size_t IRAM xPortWantedSizeAlign(size_t size)
{
    /* align the size to a multiple of 8 */
    return (size & 0x7) ? (size & ~0x7) + 8 : size;
}

size_t IRAM xPortGetFreeHeapSize (void)
{
    return get_free_heap_size ();
}

/*
 * Following functions implement the lock mechanism in newlib. The only static
 * mutex defined here is the __malloc_recursive_mutex to avoid that memory
 * management functions try to lock before RIOT's threads are running.
 * These lock/unlock functions cannot be used in the SDK version since some ISR
 * in SDK use malloc/free which does not work in interrupt context.
 */

extern _lock_t __malloc_recursive_mutex;

static rmutex_t _malloc_rmtx = RMUTEX_INIT;

void IRAM syscalls_init (void)
{
    __malloc_recursive_mutex = (_lock_t)&_malloc_rmtx;
}

void IRAM _lock_init(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL);
    CHECK_PARAM (*lock != ((_lock_t)&_malloc_rmtx));

    mutex_t* mtx = malloc (sizeof(mutex_t));

    if (mtx) {
        memset (mtx, 0, sizeof(mutex_t));
        *lock = (_lock_t)mtx;
    }
}

void IRAM _lock_init_recursive(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL);
    CHECK_PARAM (*lock != ((_lock_t)&_malloc_rmtx));

    rmutex_t* rmtx = malloc (sizeof(rmutex_t));

    if (rmtx) {
        memset (rmtx, 0, sizeof(rmutex_t));
        *lock = (_lock_t)rmtx;
    }
}

void IRAM _lock_close(_lock_t *lock)
{
    CHECK_PARAM (lock != NULL);
    CHECK_PARAM (*lock != ((_lock_t)&_malloc_rmtx));

    free ((void*)*lock);
    *lock = 0;
}

void IRAM _lock_close_recursive(_lock_t *lock)
{
    CHECK_PARAM (lock != NULL);
    CHECK_PARAM (*lock != ((_lock_t)&_malloc_rmtx));

    free ((void*)*lock);
    *lock = 0;
}

void IRAM _lock_acquire(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL && *lock != 0);

    mutex_lock ((mutex_t*)*lock);
}

void IRAM _lock_acquire_recursive(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL && *lock != 0);

    rmutex_lock ((rmutex_t*)*lock);
}

int IRAM _lock_try_acquire(_lock_t *lock)
{
    CHECK_PARAM_RET (sched_active_thread != 0, 0);
    CHECK_PARAM_RET (lock != NULL && *lock != 0, 0);

    return rmutex_trylock ((rmutex_t*)*lock);
}

int IRAM _lock_try_acquire_recursive(_lock_t *lock)
{
    CHECK_PARAM_RET (sched_active_thread != 0, 0);
    CHECK_PARAM_RET (lock != NULL && *lock != 0, 0);

    return mutex_trylock ((mutex_t*)*lock);
}

void IRAM _lock_release(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL && *lock != 0);

    mutex_unlock ((mutex_t*)*lock);
}

void IRAM _lock_release_recursive(_lock_t *lock)
{
    CHECK_PARAM (sched_active_thread != 0);
    CHECK_PARAM (lock != NULL && *lock != 0);

    rmutex_unlock ((rmutex_t*)*lock);
}

#define _cheap heap_top

extern char *heap_top;
extern char _eheap;     /* end of heap (defined in esp8266.riot-os.app.ld) */
extern char _sheap;     /* start of heap (defined in esp8266.riot-os.app.ld) */

unsigned int IRAM get_free_heap_size (void)
{
    return (_cheap) ? &_eheap - _cheap : 0;
}

void heap_stats(void)
{
    struct mallinfo minfo = mallinfo();
    ets_printf("heap: %u (free %u), ", &_eheap - &_sheap, get_free_heap_size());
    ets_printf("sysmem: %d (used %d, free %d)\n",
               minfo.arena, minfo.uordblks, minfo.fordblks);
}

#endif /* MODULE_ESP_SDK */

int _rename_r (struct _reent *r, const char* old, const char* new)
{
    DEBUG("%s: system function does not exist\n", __func__);
    r->_errno = ENOSYS;
    return -1;
}

#include <math.h>

double __ieee754_remainder(double x, double y) {
    return x - y * floor(x/y);
}

float __ieee754_remainderf(float x, float y) {
    return x - y * floor(x/y);
}
