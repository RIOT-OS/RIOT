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

#include <sys/unistd.h>

#include "esp/common_macros.h"
#include "irq_arch.h"
#include "periph_cpu.h"
#include "periph/pm.h"
#include "syscalls.h"
#include "timex.h"

#include "rom/ets_sys.h"
#include "rom/libc_stubs.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "sdk_conf.h"
#include "xtensa/xtensa_api.h"

#ifdef MODULE_ESP_IDF_HEAP
#include "esp_heap_caps.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define MHZ 1000000UL

#ifdef MODULE_ESP_IDF_HEAP

/* if module esp_idf_heap is used, this function has to be defined for ESP32 */
unsigned int get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

/* alias for compatibility with espressif/wifi_libs */
uint32_t esp_get_free_heap_size( void ) __attribute__((alias("get_free_heap_size")));

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

#endif /* MODULE_ESP_IDF_HEAP */

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
                                       _CONST char *, size_t len),
                         va_list * ap);

extern int _scanf_float(struct _reent *rptr,
                        void *pdata,
                        FILE *fp,
                        va_list *ap);

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

void IRAM syscalls_init_arch(void)
{
    /* enable the system timer in us (TMG0 is enabled by default) */
    TIMER_SYSTEM.config.divider = rtc_clk_apb_freq_get()/MHZ;
    TIMER_SYSTEM.config.autoreload = 0;
    TIMER_SYSTEM.config.enable = 1;

    syscall_table_ptr_pro = &s_stub_table;
    syscall_table_ptr_app = &s_stub_table;
}

uint32_t system_get_time(void)
{
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* read the current timer value */
    return TIMER_SYSTEM.cnt_low;
}

uint32_t system_get_time_ms(void)
{
    return system_get_time_64() / US_PER_MS;
}

uint64_t system_get_time_64(void)
{
    uint64_t  ret;
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
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
