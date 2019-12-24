/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/reent.h>
#include <sys/signal.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include "esp_common.h"
#include "cpu_conf.h"
#include "irq.h"
#include "irq_arch.h"
#include "kernel_defines.h"
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "sched.h"
#include "periph/pm.h"

#include "timex.h"

#include "esp_attr.h"
#include "esp/xtensa_ops.h"
#include "esp/common_macros.h"
#include "rom/ets_sys.h"
#include "rom/libc_stubs.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "xtensa/xtensa_api.h"

#include "periph_cpu.h"
#include "syscalls.h"

#ifdef MODULE_ESP_IDF_HEAP
#include "heap/esp_heap_caps.h"
#include "heap/include/multi_heap.h"
#else
#include "malloc.h"
#endif

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
#endif /*  MODULE_PTHREAD*/

/**
 * @name Locking functions
 *
 * Following function implements the lock mechanism in newlib. The only static
 * mutex defined here is the _malloc_rmtx to avoid that memory management
 * functions try to lock before RIOT's threads are running. All other mutexes
 * are allocated dynamically.
 */

static rmutex_t _malloc_rmtx = RMUTEX_INIT;

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

/**
 * @name Memory allocation functions
 */

#ifdef MODULE_ESP_IDF_HEAP

extern void *heap_caps_malloc_default( size_t size );
extern void *heap_caps_realloc_default( void *ptr, size_t size );

void* IRAM_ATTR __wrap__malloc_r(struct _reent *r, size_t size)
{
    return heap_caps_malloc_default( size );
}

void IRAM_ATTR __wrap__free_r(struct _reent *r, void* ptr)
{
    heap_caps_free( ptr );
}

void* IRAM_ATTR __wrap__realloc_r(struct _reent *r, void* ptr, size_t size)
{
    return heap_caps_realloc_default( ptr, size );
}

void* IRAM_ATTR __wrap__calloc_r(struct _reent *r, size_t count, size_t size)
{
    void* result = heap_caps_malloc_default(count * size);
    if (result) {
        bzero(result, count * size);
    }
    return result;
}

unsigned int IRAM get_free_heap_size (void)
{
    return heap_caps_get_free_size( MALLOC_CAP_DEFAULT );
}

void heap_stats(void)
{
    multi_heap_info_t hinfo;

    heap_caps_get_info(&hinfo,  MALLOC_CAP_DEFAULT);

    size_t _free = hinfo.total_free_bytes;
    size_t _alloc = hinfo.total_allocated_bytes;

    printf("heap: %u (used %u free %u) [bytes]\n",
           (unsigned)(_free + _alloc), (unsigned)_alloc, (unsigned)_free);
}

#else /* MODULE_ESP_IDF_HEAP */

/* for compatibility with ESP-IDF heap functions */
void* IRAM heap_caps_malloc( size_t size, uint32_t caps )
{
    (void)caps;
    return malloc(size);
}

void* IRAM heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    (void)caps;
    return calloc(n, size);
}

void* IRAM heap_caps_realloc( void *ptr, size_t size )
{
    return realloc(ptr, size);
}

extern uint8_t  _eheap;     /* end of heap (defined in esp32.common.ld) */
extern uint8_t  _sheap;     /* start of heap (defined in esp32.common.ld) */
extern uint8_t *heap_top;   /* current top of heap as defined in newlib_syscalls_default */

unsigned int IRAM get_free_heap_size (void)
{
    struct mallinfo minfo = mallinfo();
    return &_eheap - &_sheap - minfo.uordblks;
}

void heap_stats(void)
{
    printf("heap: %u (used %u, free %u) [bytes]\n", (unsigned)(&_eheap - &_sheap),
           &_eheap - &_sheap - get_free_heap_size(), get_free_heap_size());
}

#endif /* MODULE_ESP_IDF_HEAP */

/* alias for compatibility with espressif/wifi_libs */
uint32_t esp_get_free_heap_size( void ) __attribute__((alias("get_free_heap_size")));

/**
 * @name Other system functions
 */

void _abort(void)
{
    ets_printf("#! abort called: powering off\n");
    pm_off();
    while(1);
}

void _exit_r(struct _reent *r, int status)
{
    _exit(status);
}

struct _reent* __getreent(void) {
    return _GLOBAL_REENT;
}

static int _no_sys_func (struct _reent *r)
{
    DEBUG("%s: system function does not exist\n", __func__);
    r->_errno = ENOSYS;
    return -1;
}

static struct _reent s_reent;

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

void IRAM syscalls_init (void)
{
    /* enable the system timer in us (TMG0 is enabled by default) */
    TIMER_SYSTEM.config.divider = rtc_clk_apb_freq_get()/MHZ;
    TIMER_SYSTEM.config.autoreload = 0;
    TIMER_SYSTEM.config.enable = 1;
    syscall_table_ptr_pro = &s_stub_table;
    syscall_table_ptr_app = &s_stub_table;

    _GLOBAL_REENT = &s_reent;

    environ = malloc(sizeof(char*));
    environ[0] = NULL;
}

uint32_t system_get_time (void)
{
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* wait until instructions have been finished */
    __asm__ volatile ("isync");
    return TIMER_SYSTEM.cnt_low;
}

uint32_t system_get_time_ms (void)
{
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* wait until instructions have been finished */
    __asm__ volatile ("isync");
    return TIMER_SYSTEM.cnt_low / USEC_PER_MSEC;
}

uint64_t system_get_time_64 (void)
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

void IRAM system_wdt_feed (void)
{
    DEBUG("%s\n", __func__);
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_feed=1;                        /* reset MWDT */
    TIMERG0.wdt_wprotect=0;                    /* enable write protection */
}

void system_wdt_init (void)
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

void system_wdt_stop (void)
{
    xt_ints_off(BIT(CPU_INUM_WDT));
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 0;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
}

void system_wdt_start (void)
{
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 1;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
    xt_ints_on(BIT(CPU_INUM_WDT));
}

__attribute__((weak)) void
_system_prevent_memset_lto(void *const  s, int c, const size_t n)
{
    (void) s;
    (void) c;
    (void) n;
}

void *system_secure_memset(void *s, int c, size_t n)
{
    memset(s, c, n);
    _system_prevent_memset_lto(s, c, n);
    return s;
}
