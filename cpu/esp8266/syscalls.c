/*
 * Copyright (C) 2019 Gunar Schorcht
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

#include "periph/pm.h"
#include "timex.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include "mutex.h"
#include "rmutex.h"

#include "esp_common.h"
#include "esp/common_macros.h"
#include "esp_attr.h"
#include "sdk/sdk.h"
#include "syscalls.h"

#ifdef MODULE_STDIO_UART
#include "stdio_uart.h"
#endif

#ifdef MODULE_ESP_IDF_HEAP
#include "esp_heap_caps.h"
#else
#include "malloc.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MHZ 1000000UL

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

#if 0

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

void IRAM _lock_init(_lock_t *lock)
{
    assert(lock != NULL);

    mutex_t* mtx = malloc(sizeof(mutex_t));

    if (mtx) {
        memset(mtx, 0, sizeof(mutex_t));
        *lock = (_lock_t)mtx;
    }
}

void IRAM _lock_init_recursive(_lock_t *lock)
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

void IRAM _lock_close(_lock_t *lock)
{
    assert(lock != NULL);
    assert(lock != __malloc_static_object);

    free((void*)*lock);
    *lock = 0;
}

void IRAM _lock_close_recursive(_lock_t *lock)
{
    assert(lock != NULL);
    assert(lock != __malloc_static_object);

    free((void*)*lock);
    *lock = 0;
}

void IRAM _lock_acquire(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init(lock);
    }

    /* if scheduler is not running, we have not to lock the mutex */
    if (sched_active_thread == NULL) {
        return;
    }

    assert(!irq_is_in());
    mutex_lock((mutex_t*)*lock);
}

void IRAM _lock_acquire_recursive(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init_recursive(lock);
    }

    /* if scheduler is not running, we have not to lock the rmutex */
    if (sched_active_thread == NULL) {
        return;
    }

    assert(!irq_is_in());
    rmutex_lock((rmutex_t*)*lock);
}

int IRAM _lock_try_acquire(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init(lock);
    }

    /* if scheduler is not running, we have not to lock the mutex */
    if (sched_active_thread == NULL) {
        return 0;
    }

    if (irq_is_in()) {
        return 0;
    }

    return mutex_trylock((mutex_t*)*lock);
}

int IRAM _lock_try_acquire_recursive(_lock_t *lock)
{
    assert(lock != NULL);

    /* if the lock data structure is still not allocated, initialize it first */
    if (*lock == 0) {
        _lock_init_recursive(lock);
    }

    /* if scheduler is not running, we have not to lock the rmutex */
    if (sched_active_thread == NULL) {
        return 0;
    }

    if (irq_is_in()) {
        return 0;
    }

    return rmutex_trylock((rmutex_t*)*lock);
}

void IRAM _lock_release(_lock_t *lock)
{
    assert(lock != NULL && *lock != 0);

    /* if scheduler is not running, we have not to unlock the mutex */
    if (sched_active_thread == NULL) {
        return;
    }

    mutex_unlock((mutex_t*)*lock);
}

void IRAM _lock_release_recursive(_lock_t *lock)
{
    assert(lock != NULL && *lock != 0);

    /* if scheduler is not running, we have not to unlock the rmutex */
    if (sched_active_thread == NULL) {
        return;
    }

    rmutex_unlock((rmutex_t*)*lock);
}

#endif

/**
 * @name Memory allocation functions
 */

#ifdef MODULE_ESP_IDF_HEAP

#define MALLOC_CAP_DEFAULT                  MALLOC_CAP_8BIT
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

unsigned int get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

void heap_stats(void)
{
    extern heap_region_t g_heap_region[HEAP_REGIONS_MAX];

    for (int i = 0; i < HEAP_REGIONS_MAX; i++) {
        ets_printf("Heap region %u @%p: %u (used %u, free %u) [bytes]\n", i,
                   g_heap_region[i].start_addr,
                   g_heap_region[i].total_size,
                   g_heap_region[i].total_size - g_heap_region[i].free_bytes,
                   g_heap_region[i].free_bytes);
    }
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

extern uint8_t  _eheap;     /* end of heap (defined in esp32.common.ld) */
extern uint8_t  _sheap;     /* start of heap (defined in esp32.common.ld) */

unsigned int IRAM get_free_heap_size(void)
{
    struct mallinfo minfo = mallinfo();
    return &_eheap - &_sheap - minfo.uordblks;
}

void heap_stats(void)
{
    ets_printf("heap: %u (used %u, free %u) [bytes]\n",
               &_eheap - &_sheap, &_eheap - &_sheap - get_free_heap_size(),
               get_free_heap_size());
}

/* alias for compatibility with espressif/wifi_libs */
uint32_t esp_get_free_heap_size( void ) __attribute__((alias("get_free_heap_size")));

#endif /* MODULE_ESP_IDF_HEAP */

/**
 * @name Other system functions
 */

int _rename_r(struct _reent *r, const char *from, const char *to)
{
    return 0;
}

void _abort(void)
{
    ets_printf("#! abort called: powering off\n");
    pm_off();
    while (1) { };
}

void _exit_r(struct _reent *r, int status)
{
    _exit(status);
}

struct _reent* __getreent(void) {
    return _GLOBAL_REENT;
}

#ifdef MCU_ESP32
static int _no_sys_func(struct _reent *r)
{
    DEBUG("%s: system function does not exist\n", __func__);
    r->_errno = ENOSYS;
    return -1;
}
#endif

static struct _reent s_reent;

#ifdef MCU_ESP32
static struct syscall_stub_table s_stub_table =
{
    .__getreent = &__getreent,

    ._malloc_r = &_malloc_r,
    ._free_r = &_free_r,
    ._realloc_r = &_realloc_r,
    ._calloc_r = &_calloc_r,
    ._sbrk_r = &_sbrk_r,

    ._system_r = (int (*)(struct _reent *, const char*))&_no_sys_func,
    ._raise_r = (void (*)(struct _reent *))&_no_sys_func,
    ._abort = &_abort,
    ._exit_r = &_exit_r,
    ._getpid_r = &_getpid_r,
    ._kill_r = &_kill_r,

    ._times_r = &_times_r,
    ._gettimeofday_r = _gettimeofday_r,

    ._open_r = &_open_r,
    ._close_r = &_close_r,
    ._lseek_r = (int (*)(struct _reent *r, int, int, int))&_lseek_r,
    ._fstat_r = &_fstat_r,
    ._stat_r = &_stat_r,
    ._write_r = (int (*)(struct _reent *r, int, const void *, int))&_write_r,
    ._read_r = (int (*)(struct _reent *r, int, void *, int))&_read_r,
    ._unlink_r = &_unlink_r,
    ._link_r = (int (*)(struct _reent *r, const char*, const char*))&_no_sys_func,
    ._rename_r = (int (*)(struct _reent *r, const char*, const char*))&_no_sys_func,

    ._lock_init = &_lock_init,
    ._lock_init_recursive = &_lock_init_recursive,
    ._lock_close = &_lock_close,
    ._lock_close_recursive = &_lock_close_recursive,
    ._lock_acquire = &_lock_acquire,
    ._lock_acquire_recursive = &_lock_acquire_recursive,
    ._lock_try_acquire = &_lock_try_acquire,
    ._lock_try_acquire_recursive = &_lock_try_acquire_recursive,
    ._lock_release = &_lock_release,
    ._lock_release_recursive = &_lock_release_recursive,

    #if CONFIG_NEWLIB_NANO_FORMAT
    ._printf_float = &_printf_float,
    ._scanf_float = &_scanf_float,
    #else /* CONFIG_NEWLIB_NANO_FORMAT */
    ._printf_float = NULL,
    ._scanf_float = NULL,
    #endif /* CONFIG_NEWLIB_NANO_FORMAT */
};
#endif

void syscalls_init(void)
{
#ifdef MCU_ESP32
    /* enable the system timer in us (TMG0 is enabled by default) */
    TIMER_SYSTEM.config.divider = rtc_clk_apb_freq_get()/MHZ;
    TIMER_SYSTEM.config.autoreload = 0;
    TIMER_SYSTEM.config.enable = 1;
    syscall_table_ptr_pro = &s_stub_table;
    syscall_table_ptr_app = &s_stub_table;
#endif
    _GLOBAL_REENT = &s_reent;

    environ = malloc(sizeof(char*));
    environ[0] = NULL;

    /*
     * initialization of newlib, includes the ctors initialization
     */
    extern void __libc_init_array(void);
    __libc_init_array();
}

uint32_t system_get_time(void)
{
    return phy_get_mactime();
}

uint32_t system_get_time_ms(void)
{
    return phy_get_mactime() / US_PER_MS;
}

#ifdef MCU_ESP32
uint64_t system_get_time_64(void)
{
    uint64_t  ret;
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* wait until instructions have been finished */
    __asm__ volatile ("isync");
    /* read the current timer value */
    ret  = TIMER_SYSTEM.cnt_low;
    ret += ((uint64_t)TIMER_SYSTEM.cnt_high) << 32;
    return ret;
}

/* alias for compatibility with espressif/wifi_libs */
int64_t esp_timer_get_time(void) __attribute__((alias("system_get_time_64")));

static IRAM void system_wdt_int_handler(void *arg)
{
    TIMERG0.int_clr_timers.wdt=1; /* clear interrupt */
    system_wdt_feed();
}

void IRAM system_wdt_feed(void)
{
    DEBUG("%s\n", __func__);
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_feed=1;                        /* reset MWDT */
    TIMERG0.wdt_wprotect=0;                    /* enable write protection */
}

void system_wdt_init(void)
{
    /* disable boot watchdogs */
    TIMERG0.wdt_config0.flashboot_mod_en = 0;
    RTCCNTL.wdt_config0.flashboot_mod_en = 0;

    /* enable system watchdog */
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.stg0 = TIMG_WDT_STG_SEL_INT;          /* stage0 timeout: interrupt */
    TIMERG0.wdt_config0.stg1 = TIMG_WDT_STG_SEL_RESET_SYSTEM; /* stage1 timeout: sys reset */
    TIMERG0.wdt_config0.sys_reset_length = 7;  /* sys reset signal length: 3.2 us */
    TIMERG0.wdt_config0.cpu_reset_length = 7;  /* sys reset signal length: 3.2 us */
    TIMERG0.wdt_config0.edge_int_en = 0;
    TIMERG0.wdt_config0.level_int_en = 1;

    /* MWDT clock = 80 * 12,5 ns = 1 us */
    TIMERG0.wdt_config1.clk_prescale = 80;

    /* define stage timeouts */
    TIMERG0.wdt_config2 = 2 * US_PER_SEC;  /* stage 0: 2 s (interrupt) */
    TIMERG0.wdt_config3 = 4 * US_PER_SEC;  /* stage 1: 4 s (sys reset) */

    TIMERG0.wdt_config0.en = 1;   /* enable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */

    DEBUG("%s TIMERG0 wdt_config0=%08x wdt_config1=%08x wdt_config2=%08x\n",
          __func__, TIMERG0.wdt_config0.val, TIMERG0.wdt_config1.val,
          TIMERG0.wdt_config2);

    /* route WDT peripheral interrupt source to CPU_INUM_WDT */
    intr_matrix_set(PRO_CPU_NUM, ETS_TG0_WDT_LEVEL_INTR_SOURCE, CPU_INUM_WDT);
    /* set the interrupt handler and activate the interrupt */
    xt_set_interrupt_handler(CPU_INUM_WDT, system_wdt_int_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_WDT));
}

void system_wdt_stop(void)
{
    xt_ints_off(BIT(CPU_INUM_WDT));
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 0;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
}

void system_wdt_start(void)
{
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 1;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
    xt_ints_on(BIT(CPU_INUM_WDT));
}
#endif

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
