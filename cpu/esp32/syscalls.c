/*
 * Copyright (C) 2022 Gunar Schorcht
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
#include <stdint.h>
#include <sys/unistd.h>
#include <sys/time.h>

#include "div.h"
#include "esp/common_macros.h"
#include "irq_arch.h"
#include "mutex.h"
#include "rmutex.h"
#include "periph_cpu.h"
#include "periph/pm.h"
#include "syscalls.h"
#include "sys/lock.h"
#include "timex.h"

#include "esp_clk_tree.h"
#include "esp_cpu.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_caps.h"
#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "hal/wdt_hal.h"
#include "hal/wdt_types.h"
#include "rom/ets_sys.h"
#include "rom/libc_stubs.h"
#include "soc/periph_defs.h"
#include "soc/rtc.h"
#ifndef CPU_FAM_ESP32H2
#  include "soc/rtc_cntl_reg.h"
#  include "soc/rtc_cntl_struct.h"
#endif
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "sdkconfig.h"
#if __xtensa__
#include "xtensa/xtensa_api.h"
#endif

#ifdef MODULE_ESP_IDF_HEAP
#include "esp_heap_caps.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_CPP)
/* weak function that have to be overridden, otherwise DEFAULT_ARENA_SIZE would
 * be allocated that would consume the whole heap memory */
size_t __cxx_eh_arena_size_get(void)
{
    return 0;
}
#endif

#if IS_USED(MODULE_ESP_IDF_HEAP)

/* if module esp_idf_heap is used, this function has to be defined for ESP32 */
unsigned int get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

/* this function is platform specific if module esp_idf_heap is used */
void heap_stats(void)
{
    size_t _free = 0;
    size_t _alloc = 0;

    multi_heap_info_t hinfo;

    heap_caps_get_info(&hinfo,  MALLOC_CAP_DEFAULT);

    _free = hinfo.total_free_bytes;
    _alloc = hinfo.total_allocated_bytes;

    ets_printf("heap: %u (used %u, free %u) [bytes]\n",
               _alloc + _free, _alloc, _free);
}

#else /* IS_USED(MODULE_ESP_IDF_HEAP) */

void *heap_caps_malloc_prefer(size_t size, size_t num, ...)
{
    /* This function usually allocates a chunk of memory in descending order
     * of the capabilities as defined in variable parameters. However,
     * allocating memory according to given capabilities is only relevant
     * if multiple heaps use memories of different capabalities, like the
     * alignment, the memory type a.s.o. Since we only use embedded RAM with
     * identical capabilities, we just map this function to the standard malloc.
     */
    return malloc(size);
}

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps)
{
    (void)alignment;
    (void)caps;

    return calloc(n, size);
}

#endif /* IS_USED(MODULE_ESP_IDF_HEAP) */

/**
 * @name Other system functions
 */

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

#if !IS_USED(MODULE_VFS)
int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
                              __attribute__((weak, alias("_no_sys_func")));
#endif

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME (clockid_t)1
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC (clockid_t)4
#endif

int clock_gettime_r(struct _reent *r, clockid_t clock_id, struct timespec *tp)
{
   if (tp == NULL) {
        r->_errno = EINVAL;
        return -1;
    }

    struct timeval tv;
    uint64_t now = 0;

    switch (clock_id) {
    case CLOCK_REALTIME:
        if (_gettimeofday_r(r, &tv, NULL))  {
            return -1;
        }
        tp->tv_sec = tv.tv_sec;
        tp->tv_nsec = tv.tv_usec * NS_PER_US;
        break;
    case CLOCK_MONOTONIC:
        now = system_get_time_64();
        tp->tv_sec = div_u64_by_1000000(now);
        tp->tv_nsec = (now - (tp->tv_sec * US_PER_SEC)) * NS_PER_US;
        break;
    default:
        r->_errno = EINVAL;
        return -1;
    }

    return 0;
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    return clock_gettime_r(_GLOBAL_REENT, clock_id, tp);
}

#if !IS_USED(MODULE_LIBC_GETTIMEOFDAY)
int IRAM _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    (void) tz;
    uint64_t now = system_get_time_64();
    tv->tv_sec = div_u64_by_1000000(now);
    tv->tv_usec = now - (tv->tv_sec * US_PER_SEC);
    return 0;
}
#endif

static int _no_sys_func(struct _reent *r)
{
    DEBUG("%s: system function does not exist\n", __func__);
    r->_errno = ENOSYS;
    return -1;
}

extern int _printf_float(struct _reent *rptr,
                         void *pdata,
                         FILE * fp,
                         int (*pfunc) (struct _reent *, FILE *,
                                       const char *, size_t len),
                         va_list * ap);

extern int _scanf_float(struct _reent *rptr,
                        void *pdata,
                        FILE *fp,
                        va_list *ap);

#if !defined(CPU_FAM_ESP32) && !defined(CPU_FAM_ESP32S2)
/* We need to override these functions that are used from ROM */

__attribute__((__noreturn__))
static void _riot__assert_func(const char *file, int line, const char * func,
                               const char *failedexpr)
{
    extern __NORETURN void _assert_failure(const char *file, unsigned line);
    (void)func;
    (void)failedexpr;
    _assert_failure(file, line);
}

static void _riot__sinit(struct _reent *reent)
{
    /* this function should never be called from ROM */
    assert(0);
}

#endif

static struct syscall_stub_table s_stub_table =
{
    .__getreent = &__getreent,

    ._malloc_r = (void * (*)(struct _reent *, size_t))&_malloc_r,
    ._free_r = (void (*)(struct _reent *, void *))&_free_r,
    ._realloc_r = (void * (*)(struct _reent *, void *, size_t))&_realloc_r,
    ._calloc_r = (void * (*)(struct _reent *, size_t,  size_t))&_calloc_r,
    ._sbrk_r = &_sbrk_r,

    ._system_r = (void*)&_no_sys_func,
    ._raise_r = (void*)&_no_sys_func,
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
    ._link_r = (void*)&_no_sys_func,
    ._rename_r = (void*)&_no_sys_func,

#if !defined(ESP_ROM_HAS_RETARGETABLE_LOCKING)
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
#else
    ._retarget_lock_init = &__retarget_lock_init,
    ._retarget_lock_init_recursive = &__retarget_lock_init_recursive,
    ._retarget_lock_close = &__retarget_lock_close,
    ._retarget_lock_close_recursive = &__retarget_lock_close_recursive,
    ._retarget_lock_acquire = &__retarget_lock_acquire,
    ._retarget_lock_acquire_recursive = &__retarget_lock_acquire_recursive,
    ._retarget_lock_try_acquire = &__retarget_lock_try_acquire,
    ._retarget_lock_try_acquire_recursive = &__retarget_lock_try_acquire_recursive,
    ._retarget_lock_release = &__retarget_lock_release,
    ._retarget_lock_release_recursive = &__retarget_lock_release_recursive,
#endif
/* if mpaland-printf is used, do not keep references to _printf_float here to
 * not pull in two stdio implementations */
#if CONFIG_NEWLIB_NANO_FORMAT && !MODULE_MPALAND_PRINTF
    ._printf_float = &_printf_float,
    ._scanf_float = &_scanf_float,
#else /* CONFIG_NEWLIB_NANO_FORMAT */
    ._printf_float = NULL,
    ._scanf_float = NULL,
#endif /* CONFIG_NEWLIB_NANO_FORMAT */
#if !defined(CPU_FAM_ESP32) && !defined(CPU_FAM_ESP32S2)
    .__assert_func = &_riot__assert_func,
    .__sinit = &_riot__sinit,
    ._cleanup_r = NULL,             /* we don't need a cleanup of stdio */
#endif
};

timer_hal_context_t sys_timer = {
    .dev = TIMER_LL_GET_HW(TIMER_SYSTEM_GROUP),
    .timer_id = TIMER_SYSTEM_INDEX,
};

#if defined(_RETARGETABLE_LOCKING)

/* all locking variables share the same mutex respectively the same rmutex */
static mutex_t  s_shared_mutex = MUTEX_INIT;
static rmutex_t s_shared_rmutex = RMUTEX_INIT;

/* definition of locks required by the newlib if retargetable locking is used */
extern struct __lock __attribute__((alias("s_shared_rmutex"))) __lock___sinit_recursive_mutex;
extern struct __lock __attribute__((alias("s_shared_rmutex"))) __lock___sfp_recursive_mutex;
extern struct __lock __attribute__((alias("s_shared_rmutex"))) __lock___atexit_recursive_mutex;
extern struct __lock __attribute__((alias("s_shared_rmutex"))) __lock___malloc_recursive_mutex;
extern struct __lock __attribute__((alias("s_shared_rmutex"))) __lock___env_recursive_mutex;
extern struct __lock __attribute__((alias("s_shared_mutex"))) __lock___at_quick_exit_mutex;
extern struct __lock __attribute__((alias("s_shared_mutex"))) __lock___tz_mutex;
extern struct __lock __attribute__((alias("s_shared_mutex"))) __lock___dd_hash_mutex;
extern struct __lock __attribute__((alias("s_shared_mutex"))) __lock___arc4random_mutex;

/* map newlib's `__retarget_*` functions to the existing `_lock_*` functions */

void __retarget_lock_init(_LOCK_T *lock)
{
    _lock_init(lock);
}

extern void __retarget_lock_init_recursive(_LOCK_T *lock)
{
    _lock_init_recursive(lock);
}

void __retarget_lock_close(_LOCK_T lock)
{
    _lock_close(&lock);
}

void __retarget_lock_close_recursive(_LOCK_T lock)
{
    _lock_close_recursive(&lock);
}

void __retarget_lock_acquire(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared mutex if lock is NULL */
        lock = (_lock_t)&s_shared_mutex;
    }
    _lock_acquire(&lock);
}

void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared rmutex if lock is NULL */
        lock = (_lock_t)&s_shared_rmutex;
    }
    _lock_acquire_recursive(&lock);
}

int __retarget_lock_try_acquire(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared mutex if lock is NULL */
        lock = (_lock_t)&s_shared_mutex;
    }
    return _lock_try_acquire(&lock);
}

int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared rmutex if lock is NULL */
        lock = (_lock_t)&s_shared_rmutex;
    }
    return _lock_try_acquire_recursive(&lock);
}

void __retarget_lock_release(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared mutex if lock is NULL */
        lock = (_lock_t)&s_shared_mutex;
    }
    _lock_release(&lock);
}

void __retarget_lock_release_recursive(_LOCK_T lock)
{
    if (lock == NULL) {
        /* use the shared rmutex if lock is NULL */
        lock = (_lock_t)&s_shared_rmutex;
    }
    _lock_release(&lock);
}

#endif /* _RETARGETABLE_LOCKING */

void IRAM syscalls_init_arch(void)
{
#if 0
    /* In ESP-IDF, the newlibc functions in ROM are used that require some
     * variables that have to be set to the shared mutex/rmutex. Since we
     * don't use the newlib functions in ROM, we don't have to set these
     * variables here for the moment
     */
#ifdef CONFIG_IDF_TARGET_ESP32
    /* Newlib 2.2.0 is used in ROM, the following lock symbols are defined: */
    extern _lock_t __sfp_lock;
    __sfp_lock = (_lock_t) &s_shared_rmutex;
    extern _lock_t __sinit_lock;
    __sinit_lock = (_lock_t) &s_shared_rmutex;
    extern _lock_t __env_lock_object;
    __env_lock_object = (_lock_t) &s_shared_rmutex;
    extern _lock_t __tz_lock_object;
    __tz_lock_object = (_lock_t) &s_shared_rmutex;
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    /* Newlib 3.0.0 is used in ROM, the following lock symbols are defined: */
    extern _lock_t __sinit_recursive_mutex;
    __sinit_recursive_mutex = (_lock_t) &s_shared_rmutex;
    extern _lock_t __sfp_recursive_mutex;
    __sfp_recursive_mutex = (_lock_t) &s_shared_rmutex;
#endif
#endif

    /* initialize and enable the system timer in us */
    periph_module_enable(PERIPH_TIMG0_MODULE);

    uint32_t clk_freq;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)GPTIMER_CLK_SRC_DEFAULT,
                                 ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                 &clk_freq);

    timer_ll_set_clock_source(sys_timer.dev, sys_timer.timer_id, GPTIMER_CLK_SRC_DEFAULT);
    timer_ll_enable_clock(sys_timer.dev, sys_timer.timer_id, true);
    timer_ll_set_clock_prescale(sys_timer.dev, sys_timer.timer_id, clk_freq / MHZ);
    timer_ll_set_count_direction(sys_timer.dev, sys_timer.timer_id, GPTIMER_COUNT_UP);
    timer_ll_enable_auto_reload(sys_timer.dev, sys_timer.timer_id, false);
    timer_ll_enable_counter(sys_timer.dev, sys_timer.timer_id, true);
    timer_ll_enable_alarm(sys_timer.dev, sys_timer.timer_id, false);
#if SOC_TIMER_SUPPORT_ETM
    timer_ll_enable_etm(sys_timer.dev, true);
#endif

#if defined(CPU_FAM_ESP32)
    syscall_table_ptr_pro = &s_stub_table;
    syscall_table_ptr_app = &s_stub_table;
#elif defined(CPU_FAM_ESP32S2)
    syscall_table_ptr_pro = &s_stub_table;
#else
    syscall_table_ptr = &s_stub_table;
#endif

    /* we don't need a cleanup for stdio */
    _GLOBAL_REENT->__cleanup = NULL;
    /* init stdio */
    _REENT_SDIDINIT(_GLOBAL_REENT) = 0;
    __sinit(_GLOBAL_REENT);
    _REENT_SDIDINIT(_GLOBAL_REENT) = 1;
}

uint32_t system_get_time(void)
{
    return system_get_time_64();
}

uint32_t system_get_time_ms(void)
{
    return system_get_time_64() / US_PER_MS;
}

uint64_t system_get_time_64(void)
{
    timer_ll_trigger_soft_capture(sys_timer.dev, sys_timer.timer_id);
    return timer_ll_get_counter_value(sys_timer.dev, sys_timer.timer_id);
}

wdt_hal_context_t mwdt;
wdt_hal_context_t rwdt;

static IRAM void system_wdt_int_handler(void *arg)
{
    wdt_hal_handle_intr(&mwdt);
    wdt_hal_write_protect_disable(&mwdt);
    wdt_hal_feed(&mwdt);
    wdt_hal_write_protect_enable(&mwdt);
}

void IRAM system_wdt_feed(void)
{
    wdt_hal_write_protect_disable(&mwdt);
    wdt_hal_feed(&mwdt);
    wdt_hal_write_protect_enable(&mwdt);
}

void system_wdt_init(void)
{
    /* initialize and disable boot watchdogs MWDT and RWDT (the prescaler for
     * MWDT is the APB clock in MHz to get a microsecond tick, for RWDT it is
     * not applicable) */
    uint32_t clk_freq;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)GPTIMER_CLK_SRC_DEFAULT,
                                 ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                 &clk_freq);

    wdt_hal_init(&mwdt, WDT_MWDT0, clk_freq / MHZ, true);
    wdt_hal_init(&rwdt, WDT_RWDT, 0, false);

    /* disable write protection for MWDT and RWDT */
    wdt_hal_write_protect_disable(&mwdt);
    wdt_hal_write_protect_disable(&rwdt);

    /* configure stages */
    wdt_hal_config_stage(&mwdt, WDT_STAGE0, 2 * US_PER_SEC, WDT_STAGE_ACTION_INT);
    wdt_hal_config_stage(&mwdt, WDT_STAGE1, 2 * US_PER_SEC, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&mwdt, WDT_STAGE2, 0, WDT_STAGE_ACTION_OFF);
    wdt_hal_config_stage(&mwdt, WDT_STAGE3, 0, WDT_STAGE_ACTION_OFF);

    /* enable the watchdog */
    wdt_hal_enable(&mwdt);

    /* enable write protection for MWDT and RWDT */
    wdt_hal_write_protect_enable(&mwdt);
    wdt_hal_write_protect_enable(&rwdt);

    DEBUG("%s TIMERG0 wdtconfig0=%08"PRIx32" wdtconfig1=%08"PRIx32
          " wdtconfig2=%08"PRIx32" wdtconfig3=%08"PRIx32
          " wdtconfig4=%08"PRIx32" regclk=%08"PRIx32"\n", __func__,
          TIMERG0.wdtconfig0.val, TIMERG0.wdtconfig1.val,
          TIMERG0.wdtconfig2.val, TIMERG0.wdtconfig3.val,
          TIMERG0.wdtconfig4.val, TIMERG0.regclk.val);

    /* route WDT peripheral interrupt source to CPU_INUM_WDT */
    intr_matrix_set(PRO_CPU_NUM, ETS_TG0_WDT_LEVEL_INTR_SOURCE, CPU_INUM_WDT);
    /* set the interrupt handler and activate the interrupt */
    esp_cpu_intr_set_handler(CPU_INUM_WDT, system_wdt_int_handler, NULL);
    esp_cpu_intr_enable(BIT(CPU_INUM_WDT));
}

void system_wdt_stop(void)
{
    esp_cpu_intr_disable(BIT(CPU_INUM_WDT));
    wdt_hal_write_protect_disable(&mwdt);
    wdt_hal_disable(&mwdt);
    wdt_hal_write_protect_enable(&mwdt);
}

void system_wdt_start(void)
{
    wdt_hal_write_protect_disable(&mwdt);
    wdt_hal_enable(&mwdt);
    wdt_hal_write_protect_enable(&mwdt);
    esp_cpu_intr_enable(BIT(CPU_INUM_WDT));
}

#ifndef MODULE_POSIX_SLEEP

int usleep(useconds_t us)
{
    extern void esp_rom_delay_us(uint32_t us);
    esp_rom_delay_us((uint32_t) us);
    return 0;
}

#endif
