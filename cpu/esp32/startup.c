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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jens Alfke <jens@mooseyard.com>
 * @}
 */

#include "esp_common.h"

#include <stdlib.h>
#include <string.h>
#include <sys/reent.h>

/* RIOT headers have to be included before ESP-IDF headers! */
#include "board.h"
#include "esp/common_macros.h"
#include "exceptions.h"
#include "irq_arch.h"
#include "kernel_defines.h"
#include "kernel_init.h"
#include "log.h"
#include "periph_cpu.h"
#include "stdio_base.h"
#include "syscalls.h"
#include "thread_arch.h"
#include "tools.h"

#include "periph/cpuid.h"
#include "periph/init.h"
#include "periph/rtc.h"

/* ESP-IDF headers */
#include "driver/periph_ctrl.h"
#include "esp_attr.h"
#include "esp_clk_internal.h"
#include "esp_heap_caps_init.h"
#include "esp_log.h"
#include "esp_private/startup_internal.h"
#include "esp_private/esp_clk.h"
#include "esp_rom_uart.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/apb_ctrl_reg.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_struct.h"

#if __xtensa__
#include "soc/dport_reg.h"
#include "soc/dport_access.h"
#include "xtensa/core-macros.h"
#include "xtensa/xtensa_api.h"
#endif

#if IS_USED(MODULE_ESP_SPI_RAM)
#include "spiram.h"
#endif

#if IS_USED(MODULE_PUF_SRAM)
#include "puf_sram.h"
#endif

#if IS_USED(MODULE_STDIO_UART)
#include "stdio_uart.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

#if IS_USED(MODULE_ESP_LOG_STARTUP)
#define LOG_STARTUP(format, ...) LOG_TAG_EARLY(LOG_INFO, D, __func__, format, ##__VA_ARGS__)
#else
#define LOG_STARTUP(format, ...)
#endif

/* following variables are defined in linker script */
extern uint8_t _bss_start;
extern uint8_t _bss_end;
extern uint8_t _sheap;
extern uint8_t _eheap;

extern uint8_t _rtc_bss_start;
extern uint8_t _rtc_bss_end;
extern uint8_t _rtc_bss_rtc_start;
extern uint8_t _rtc_bss_rtc_end;
extern uint8_t _iram_start;

/* external esp function declarations */
extern uint32_t hwrand (void);

/* forward declarations */
static void IRAM system_startup_cpu0(void);
static void IRAM system_init(void);
extern void IRAM_ATTR thread_yield_isr(void* arg);

uint64_t g_startup_time = 0;
const sys_startup_fn_t g_startup_fn[1] = { system_startup_cpu0 };

#if CONFIG_ESP_TIMER_IMPL_FRC2
/* dummy function required if FRC2 (legacy) timer of the ESP32 is used */
esp_err_t esp_timer_impl_early_init(void)
{
    return ESP_OK;
}
#endif

/**
 * @brief   System startup function
 *
 * This function is the entry point in the user application. It is called
 * after a CPU initialization to startup the system.
 */
static NORETURN void IRAM system_startup_cpu0(void)
{
#if __xtensa__
    register uint32_t *sp __asm__ ("a1"); (void)sp;
#endif
#if __riscv
    register uint32_t *sp __asm__ ("x2"); (void)sp;
#endif

#ifdef MODULE_PUF_SRAM
    puf_sram_init((uint8_t *)&_sheap, SEED_RAM_LEN);
#endif

#if IS_USED(MODULE_ESP_IDF_HEAP)
    /* init heap */
    heap_caps_init();
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        ets_printf("Heap free: %u byte\n", get_free_heap_size());
    }
#endif

    /* initialize system call tables of ESP32x rom and newlib */
    syscalls_init();

    /* systemwide UART initialization */
    extern void uart_system_init (void);
    uart_system_init();

    /* initialize stdio */
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    early_init();

    RESET_REASON reset_reason = rtc_get_reset_reason(PRO_CPU_NUM);

    /* initialize RTC data after power on or RTC WDT reset */
    if (reset_reason == POWERON_RESET || reset_reason == RTCWDT_RTC_RESET) {
        /* cppcheck-suppress comparePointers */
        memset(&_rtc_bss_rtc_start, 0, (&_rtc_bss_rtc_end - &_rtc_bss_rtc_start));
    }

    uint8_t cpu_id[CPUID_LEN];
    cpuid_get ((void*)cpu_id);

#if IS_USED(MODULE_ESP_LOG_STARTUP)
    LOG_STARTUP("\nStarting ESP32x with ID: ");
    for (unsigned i = 0; i < CPUID_LEN; i++) {
        ets_printf("%02x", cpu_id[i]);
    }
    ets_printf("\n");

    extern char* esp_get_idf_version(void);
    LOG_STARTUP("ESP-IDF SDK Version %s\n\n", esp_get_idf_version());
#endif

    if (reset_reason == DEEPSLEEP_RESET) {
        /* the cause has to be read to clear it */
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        (void)cause;
        LOG_STARTUP("Restart after deep sleep, wake-up cause: %d\n", cause);
    }

    LOG_STARTUP("Current clocks in Hz: CPU=%d APB=%d XTAL=%d SLOW=%d\n",
                esp_clk_cpu_freq(),
                esp_clk_apb_freq(), esp_clk_xtal_freq(),
                rtc_clk_slow_freq_get_hz());

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        ets_printf("reset reason: %d\n", reset_reason);
        ets_printf("_stack      %p\n", sp);
        ets_printf("_bss_start  %p\n", &_bss_start);
        ets_printf("_bss_end    %p\n", &_bss_end);
        if (!IS_ACTIVE(MODULE_ESP_IDF_HEAP)) {
            ets_printf("_heap_start %p\n", &_sheap);
            ets_printf("_heap_end   %p\n", &_eheap);
            ets_printf("_heap_free  %u\n", get_free_heap_size());
        }
    }

    LOG_STARTUP("PRO cpu is up (single core mode, only PRO cpu is used)\n");

    /* init esp_timer implementation */
    esp_timer_early_init();

    LOG_STARTUP("PRO cpu starts user code\n");
    system_init();

    UNREACHABLE();
}

static NORETURN void IRAM system_init (void)
{
    static_assert(MAXTHREADS >= 3,
            "ESP32x SoCs require at least 3 threads, esp_timer, idle, and main");
#if defined(CPU_FAM_ESP32)
    /* enable cached read from flash */
    Cache_Read_Enable(PRO_CPU_NUM);
#endif

    /* initialize the ISR stack for usage measurements */
    thread_isr_stack_init();

    /* install exception handlers */
    init_exceptions();

    /* set log levels for SDK library outputs */
    extern void esp_log_level_set(const char* tag, esp_log_level_t level);
    esp_log_level_set("wifi", (esp_log_level_t)LOG_DEBUG);
    esp_log_level_set("gpio", (esp_log_level_t)LOG_DEBUG);

    /* init watchdogs */
    system_wdt_init();

    /* init random number generator */
    srand(hwrand());

    /* add SPI RAM to heap if enabled */
#if CONFIG_SPIRAM_SUPPORT && CONFIG_SPIRAM_BOOT_INIT
    esp_spiram_init_cache();
    esp_spiram_add_to_heapalloc();
#endif

    /* print some infos */
    LOG_STARTUP("Used clocks in Hz: CPU=%d APB=%d XTAL=%d FAST=%d SLOW=%d\n",
                esp_clk_cpu_freq(),
                esp_clk_apb_freq(), esp_clk_xtal_freq(),
                rtc_clk_fast_freq_get() == RTC_FAST_FREQ_8M ? 8 * MHZ
                                                            : esp_clk_xtal_freq()/4,
                rtc_clk_slow_freq_get_hz());
    LOG_STARTUP("XTAL calibration value: %d\n", esp_clk_slowclk_cal_get());
    LOG_STARTUP("Heap free: %u bytes\n", get_free_heap_size());

    /* initialize architecture specific interrupt handling */
    esp_irq_init();

    /* disable buffering in stdio */
    setvbuf(_stdout_r(_REENT), NULL, _IONBF, 0);
    setvbuf(_stderr_r(_REENT), NULL, _IONBF, 0);

    /* trigger static peripheral initialization */
    periph_init();

    /* print system time */
#if IS_USED(MODULE_PERIPH_RTC)
    struct tm _sys_time;
    rtc_get_time(&_sys_time);
    LOG_STARTUP("System time: %04d-%02d-%02d %02d:%02d:%02d\n",
                _sys_time.tm_year + 1900, _sys_time.tm_mon + 1, _sys_time.tm_mday,
                _sys_time.tm_hour, _sys_time.tm_min, _sys_time.tm_sec);
#endif

    /* print the board config */
#if IS_USED(MODULE_ESP_LOG_STARTUP)
    print_board_config();
#endif

#if IS_USED(MODULE_PERIPH_FLASHPAGE)
    extern void esp_flashpage_init(void);
    esp_flashpage_init();
#endif

#if IS_USED(MODULE_MTD)
    /* init flash drive */
    extern void spi_flash_drive_init (void);
    spi_flash_drive_init();
#endif

    /* initialize the board */
    extern void board_init(void);
    board_init();

#ifndef __XTENSA__
    /* route a software interrupt source to CPU as trigger for thread yields,
     * we use an internal software interrupt on Xtensa-based ESP32x SoCs */
    intr_matrix_set(PRO_CPU_NUM, ETS_FROM_CPU_INTR0_SOURCE, CPU_INUM_SOFTWARE);
#endif
    /* set thread yield handler and enable the software interrupt */
    intr_cntrl_ll_set_int_handler(CPU_INUM_SOFTWARE, thread_yield_isr, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_SOFTWARE));

    /* initialize ESP system event loop */
    extern void esp_event_handler_init(void);
    esp_event_handler_init();

    /* initialize ESP-IDF timer task */
    esp_timer_init();

    /* starting RIOT */
#if IS_USED(MODULE_ESP_LOG_STARTUP)
    LOG_STARTUP("Starting RIOT kernel on PRO cpu\n");
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
#else
    ets_printf("\n");
#endif
    kernel_init();
    UNREACHABLE();
}
