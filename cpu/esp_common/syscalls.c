/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include <stdio_ext.h>
#include <sys/unistd.h>

#include "irq_arch.h"
#include "mutex.h"
#include "rmutex.h"
#include "timex.h"

#include "esp_attr.h"
#include "syscalls.h"

#ifdef MODULE_ESP_IDF_HEAP
#include "esp_heap_caps.h"
#else
#include "malloc.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef MODULE_PTHREAD

#define PTHREAD_CANCEL_DISABLE 1
/*
 * This is a dummy function to avoid undefined references when linking
 * against newlib and module pthread is not used.
 */
int pthread_setcancelstate(int state, int *oldstate)
{
    if (oldstate) {
        *oldstate = PTHREAD_CANCEL_DISABLE;
    }
    return 0;
}
#endif /*  MODULE_PTHREAD */

/*
 * TODO: When the lock functions in this section are enabled, an application
 * crashes when an ISR calls a `newlib` function that uses `_lock_acquire`
 * or `_log_acquire_recursive` to be thread-safe, for example, `puts` in
 * `tests/isr_yield_higher`. The reason is that the implementation of these
 * functions uses `mutex` and `rmutex` that do not work in the interrupt
 * context. Therefore, the lock functions are disabled for the moment, and
 * instead `newlib`'s dummy lock functions are used which do not guarantee
 * thread safety.
 */

/**
 * @name Locking functions
 *
 * Following functions implement the lock mechanism for newlib.
 */

/**
 * _malloc_rmtx is defined as static variable to avoid recursive calls of
 * malloc when _malloc_r tries to lock __malloc_lock_object the first
 * time. All other mutexes that are used for the lock mechanism are allocated
 * dynamically.
 */
static rmutex_t _malloc_rmtx = RMUTEX_INIT;

/**
 * To properly handle the static rmutex _malloc_rmtx, we have to know
 * the address of newlib's static variable __malloc_lock_object.
 */
static _lock_t *__malloc_static_object = NULL;

void IRAM_ATTR _lock_init(_lock_t *lock)
{
    assert(lock != NULL);

    mutex_t* mtx = malloc(sizeof(mutex_t));

    if (mtx) {
        memset(mtx, 0, sizeof(mutex_t));
        *lock = (_lock_t)mtx;
    }
}

void IRAM_ATTR _lock_init_recursive(_lock_t *lock)
{
    assert(lock != NULL);

    /**
     * Since we don't have direct access to newlib's static variable
     * __malloc_lock_object, we have to rely on the fact that function
     * _lock_aqcuire_recursive, and thus function _lock_init_recursive
     * is called for the first time with newlib's static variable
     * __malloc_lock_object as parameter. This is ensured by calling
     * malloc in the function syscalls_init.
     */
    if (__malloc_static_object == NULL) {
        *lock = (_lock_t)&_malloc_rmtx;
        __malloc_static_object = lock;
        return;
    }

    /* _malloc_rmtx is static and has not to be allocated */
    if (lock == __malloc_static_object) {
        return;
    }

    rmutex_t* rmtx = malloc(sizeof(rmutex_t));

    if (rmtx) {
        memset(rmtx, 0, sizeof(rmutex_t));
        *lock = (_lock_t)rmtx;
    }
}

void IRAM_ATTR _lock_close(_lock_t *lock)
{
    assert(lock != NULL);
    assert(lock != __malloc_static_object);

    free((void*)*lock);
    *lock = 0;
}

void IRAM_ATTR _lock_close_recursive(_lock_t *lock)
{
    assert(lock != NULL);
    assert(lock != __malloc_static_object);

    free((void*)*lock);
    *lock = 0;
}

void IRAM_ATTR _lock_acquire(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init(lock);
    }

    /* if scheduler is not running, we have not to lock the mutex */
    if (thread_get_active() == NULL) {
        return;
    }

    assert(!irq_is_in());
    mutex_lock((mutex_t*)*lock);
}

void IRAM_ATTR _lock_acquire_recursive(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init_recursive(lock);
    }

    /* if scheduler is not running, we have not to lock the rmutex */
    if (thread_get_active() == NULL) {
        return;
    }

    assert(!irq_is_in());
    rmutex_lock((rmutex_t*)*lock);
}

int IRAM_ATTR _lock_try_acquire(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init(lock);
    }

    /* if scheduler is not running, we have not to lock the mutex */
    if (thread_get_active() == NULL) {
        return 0;
    }

    if (irq_is_in()) {
        return 0;
    }

    return mutex_trylock((mutex_t*)*lock);
}

int IRAM_ATTR _lock_try_acquire_recursive(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init_recursive(lock);
    }

    /* if scheduler is not running, we have not to lock the rmutex */
    if (thread_get_active() == NULL) {
        return 0;
    }

    if (irq_is_in()) {
        return 0;
    }

    return rmutex_trylock((rmutex_t*)*lock);
}

void IRAM_ATTR _lock_release(_lock_t *lock)
{
    assert(lock != NULL && *lock != 0);

    /* if scheduler is not running, we have not to unlock the mutex */
    if (thread_get_active() == NULL) {
        return;
    }

    mutex_unlock((mutex_t*)*lock);
}

void IRAM_ATTR _lock_release_recursive(_lock_t *lock)
{
    assert(lock != NULL && *lock != 0);

    /* if scheduler is not running, we have not to unlock the rmutex */
    if (thread_get_active() == NULL) {
        return;
    }

    rmutex_unlock((rmutex_t*)*lock);
}

/**
 * @name Memory allocation functions
 */

#ifdef MODULE_ESP_IDF_HEAP

#define heap_caps_malloc_default(s)         heap_caps_malloc(s, MALLOC_CAP_DEFAULT)
#define heap_caps_realloc_default(p,s)      heap_caps_realloc(p, s, MALLOC_CAP_DEFAULT)

void* IRAM_ATTR __wrap__malloc_r(struct _reent *r, size_t size)
{
    return heap_caps_malloc_default( size );
}

void IRAM_ATTR __wrap__free_r(struct _reent *r, void *ptr)
{
    heap_caps_free( ptr );
}

void* IRAM_ATTR __wrap__realloc_r(struct _reent *r, void* ptr, size_t size)
{
    return heap_caps_realloc_default( ptr, size );
}

void* IRAM_ATTR __wrap__calloc_r(struct _reent *r, size_t count, size_t size)
{
    void *result = heap_caps_malloc_default(count * size);
    if (result) {
        bzero(result, count * size);
    }
    return result;
}

#else /* MODULE_ESP_IDF_HEAP */

/* for compatibility with ESP-IDF heap functions */

void* _heap_caps_malloc(size_t size, uint32_t caps, const char *file, size_t line)
{
    (void)caps;
    return malloc(size);
}

void* _heap_caps_calloc(size_t n, size_t size, uint32_t caps, const char *file, size_t line)
{
    (void)caps;
    return calloc(n, size);
}

void* _heap_caps_realloc(void *ptr, size_t size, uint32_t caps, const char *file, size_t line)
{
    return realloc(ptr, size);
}

void *_heap_caps_zalloc(size_t size, uint32_t caps, const char *file, size_t line)
{
    void *ptr = malloc(size);
    if (ptr)  {
        memset(ptr, 0, size);
    }
    return ptr;
}

void _heap_caps_free(void *ptr, const char *file, size_t line)
{
    (void)file;
    (void)line;
    free(ptr);
}

void heap_caps_init(void)
{
}

extern uint8_t  _eheap;     /* end of heap (defined in ld script) */
extern uint8_t  _sheap;     /* start of heap (defined in ld script) */

extern uint8_t _sheap1;
extern uint8_t _eheap1;

extern uint8_t _sheap2;
extern uint8_t _eheap2;

extern uint8_t _sheap3;
extern uint8_t _eheap3;

unsigned int IRAM_ATTR get_free_heap_size(void)
{
    struct mallinfo minfo = mallinfo();
    unsigned int heap_size = &_eheap - &_sheap;
#if NUM_HEAPS > 1
    heap_size += &_eheap1 - &_sheap1;
#endif
#if NUM_HEAPS > 2
    heap_size += &_eheap2 - &_sheap2;
#endif
#if NUM_HEAPS > 3
    heap_size += &_eheap3 - &_sheap3;
#endif
    return heap_size - minfo.uordblks;
}

/* alias for compatibility with espressif/wifi_libs */
uint32_t esp_get_free_heap_size( void ) __attribute__((alias("get_free_heap_size")));

#endif /* MODULE_ESP_IDF_HEAP */

/**
 * @name Other system functions
 */

struct _reent* __getreent(void) {
    return _GLOBAL_REENT;
}

static struct _reent s_reent;

void syscalls_init(void)
{
    extern void syscalls_init_arch(void);
    syscalls_init_arch();

    _GLOBAL_REENT = &s_reent;

    environ = malloc(sizeof(char*));
    environ[0] = NULL;

    /* initialization of newlib, includes the ctors initialization */
    extern void __libc_init_array(void);
    __libc_init_array();

    /* initialization of global reent data structure */
    _REENT_SMALL_CHECK_INIT(_GLOBAL_REENT);

    /*
     * disable the locking for stdout/stderr to avoid rmutex based locking
     * when puts/printf are called from an ISR
     */
    __fsetlocking(_GLOBAL_REENT->_stdout, FSETLOCKING_BYCALLER);
    __fsetlocking(_GLOBAL_REENT->_stderr, FSETLOCKING_BYCALLER);
}

__attribute__((weak)) void
_system_prevent_memset_lto(void *const  s, int c, const size_t n)
{
    (void)s;
    (void)c;
    (void)n;
}

void *system_secure_memset(void *s, int c, size_t n)
{
    memset(s, c, n);
    _system_prevent_memset_lto(s, c, n);
    return s;
}
