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

#include "board.h"
#include "esp_attr.h"
#include "exceptions.h"
#include "irq_arch.h"
#include "kernel_defines.h"
#include "kernel_init.h"
#include "log.h"
#include "stdio_base.h"
#include "syscalls.h"
#include "thread_arch.h"

#include "periph/cpuid.h"
#include "periph/init.h"
#include "periph/rtc.h"

#include "driver/periph_ctrl.h"
#include "esp/common_macros.h"
#include "esp32/esp_sleep.h"
#include "heap/esp_heap_caps_init.h"
#include "log/esp_log.h"
#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/apb_ctrl_reg.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/dport_access.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_struct.h"
#include "xtensa/core-macros.h"
#include "xtensa/xtensa_api.h"

#include "periph_cpu.h"
#include "tools.h"

#ifdef MODULE_STDIO_UART
#include "stdio_uart.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define MHZ 1000000UL
#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

#if MODULE_ESP_LOG_STARTUP
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
extern uint8_t _init_start;

/* external esp function declarations */
extern void esp_clk_init(void);
extern void esp_perip_clk_init(void);
extern void esp_reent_init(struct _reent* r);
extern void esp_panic_wdt_stop (void);
extern void spi_ram_init(void);
extern void spi_ram_heap_init(void);
extern uint32_t hwrand (void);
extern void bootloader_clock_configure(void);

/* forward declarations */
static void system_init(void);
static void intr_matrix_clear(void);

typedef int32_t esp_err_t;

/**
 * @brief   CPU startup function
 *
 * This function is the entry point in the user application. It is called
 * after a system reset to startup the system.
 */
NORETURN void IRAM call_start_cpu0 (void)
{
    register uint32_t *sp __asm__ ("a1"); (void)sp;

    cpu_configure_region_protection();

    /* move exception vectors to IRAM */
    asm volatile ("wsr %0, vecbase\n" ::"r"(&_init_start));

    RESET_REASON reset_reason = rtc_get_reset_reason(PRO_CPU_NUM);

    /* reset from panic handler by RWDT or TG0WDT */
    if (reset_reason == RTCWDT_SYS_RESET || reset_reason == TG0WDT_SYS_RESET) {
        esp_panic_wdt_stop();
    }

    /* Clear BSS. Please do not attempt to do any complex stuff */
    /* (like early logging) before this. */
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

    /* if we are not waking up from deep sleep, clear RTC bss */
    if (reset_reason != DEEPSLEEP_RESET) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start));
    }

    /* initialize RTC data after power on */
    if (reset_reason == POWERON_RESET || reset_reason == RTCWDT_RTC_RESET) {
        memset(&_rtc_bss_rtc_start, 0, (&_rtc_bss_rtc_end - &_rtc_bss_rtc_start));
    }

    uint8_t cpu_id[CPUID_LEN];
    cpuid_get ((void*)cpu_id);

#ifdef MODULE_ESP_LOG_STARTUP
    ets_printf("\n");
    LOG_STARTUP("Starting ESP32 with ID: ");
    for (unsigned i = 0; i < CPUID_LEN; i++) {
        ets_printf("%02x", cpu_id[i]);
    }
    ets_printf("\n");
#endif

    if (reset_reason == DEEPSLEEP_RESET) {
        /* the cause has to be read to clear it */
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        (void)cause;
        LOG_STARTUP("Restart after deep sleep, wake-up cause: %d\n", cause);
    }

    LOG_STARTUP("Current clocks in Hz: CPU=%d APB=%d XTAL=%d SLOW=%d\n",
                rtc_clk_cpu_freq_value(rtc_clk_cpu_freq_get()),
                rtc_clk_apb_freq_get(), rtc_clk_xtal_freq_get()*MHZ,
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

    /* disable APP cpu */
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);

    if (IS_ACTIVE(MODULE_ESP_IDF_HEAP)) {
        /* init heap */
        heap_caps_init();
        if (IS_ACTIVE(ENABLE_DEBUG)) {
            ets_printf("Heap free: %u byte\n", get_free_heap_size());
        }
    }

    /* init SPI RAM if enabled */
    #if CONFIG_SPIRAM_SUPPORT && CONFIG_SPIRAM_BOOT_INIT
    spi_ram_init();
    #endif

    LOG_STARTUP("PRO cpu starts user code\n");
    system_init();

    UNREACHABLE();
}

#define RTC_FAST_FREQ_8M_MHZ    8000000
#define rtc_select_slow_clk     select_rtc_slow_clk

extern uint32_t esp_clk_slowclk_cal_get(void);
extern void IRAM_ATTR rtc_select_slow_clk(rtc_slow_freq_t slow_clk);

static void IRAM system_clk_init (void)
{
    /* first initialize RTC with default configuration */
    rtc_config_t rtc_cfg = RTC_CONFIG_DEFAULT();
    rtc_init_module(rtc_cfg);

    /* configure main crystal frequency if necessary */
    if (CONFIG_ESP32_XTAL_FREQ != RTC_XTAL_FREQ_AUTO &&
        CONFIG_ESP32_XTAL_FREQ != rtc_clk_xtal_freq_get()) {
        bootloader_clock_configure();
    }

    /* set FAST_CLK to internal low power clock of 8 MHz */
    rtc_clk_fast_freq_set(RTC_FAST_FREQ_8M);

#if MODULE_ESP_RTC_TIMER_32K
    /* set SLOW_CLK to external 32.768 kHz crystal clock */
    rtc_select_slow_clk(RTC_SLOW_FREQ_32K_XTAL);
#else
    /* set SLOW_CLK to internal low power clock of 150 kHz */
    rtc_clk_32k_enable(false);
    rtc_select_slow_clk(RTC_SLOW_FREQ_RTC);
#endif

    LOG_STARTUP("Switching system clocks can lead to some unreadable characters\n");

    /* wait until UART is idle to avoid losing output */
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    /* determine configured CPU clock frequency from sdk_conf.h */
    rtc_cpu_freq_t freq;
    switch (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ) {
        case 40:  freq = RTC_CPU_FREQ_XTAL; /* derived from external crystal */
                  break;                    /* normally 40 MHz */
        case 80:  freq = RTC_CPU_FREQ_80M;  /* derived from PLL */
                  break;
        case 160: freq = RTC_CPU_FREQ_160M; /* derived from PLL */
                  break;
        case 240: freq = RTC_CPU_FREQ_240M; /* derived from PLL */
                  break;
        default:  freq = RTC_CPU_FREQ_2M;   /* frequencies <= 8 MHz are
                                               set to 2 MHz and handled later */
    }

    uint32_t freq_before = rtc_clk_cpu_freq_value(rtc_clk_cpu_freq_get()) / MHZ ;

    if (freq_before != CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ) {
        /* set configured CPU frequency */
        rtc_clk_cpu_freq_set(freq);

        /* Recalculate the ccount to make time calculation correct. */
        uint32_t freq_after = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
        XTHAL_SET_CCOUNT( XTHAL_GET_CCOUNT() * freq_after / freq_before );
    }
}

extern void IRAM_ATTR thread_yield_isr(void* arg);

static NORETURN void IRAM system_init (void)
{
    /* enable cached read from flash */
    Cache_Read_Enable(PRO_CPU_NUM);

    /* initialize the ISR stack for usage measurements */
    thread_isr_stack_init();

    /* initialize clocks (CPU_CLK, APB_CLK, SLOW and FAST) */
    system_clk_init();

    /* disable clocks of peripherals that are not needed at startup */
    esp_perip_clk_init();

    /* set configured console UART baudrate */
    const int uart_clk_freq = rtc_clk_apb_freq_get();
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    uart_div_modify(CONFIG_CONSOLE_UART_NUM,
                    (uart_clk_freq << 4) / STDIO_UART_BAUDRATE);

    /* initialize system call tables of ESP32 rom and newlib */
    syscalls_init();

    /* install exception handlers */
    init_exceptions();

    /* clear interrupt matrix */
    intr_matrix_clear();

    /* systemwide UART initialization */
    extern void uart_system_init (void);
    uart_system_init();

    /* Disable the hold flag of all RTC GPIO pins */
    RTCCNTL.hold_force.val = 0;

    /* set log levels for SDK library outputs */
    extern void esp_log_level_set(const char* tag, esp_log_level_t level);
    esp_log_level_set("wifi", LOG_DEBUG);

    /* init watchdogs */
    system_wdt_init();

    /* init random number generator */
    srand(hwrand());

    /* add SPI RAM to heap if enabled */
    #if CONFIG_SPIRAM_SUPPORT && CONFIG_SPIRAM_BOOT_INIT
    spi_ram_heap_init();
    #endif

    /* print some infos */
    LOG_STARTUP("Used clocks in Hz: CPU=%d APB=%d XTAL=%d FAST=%d SLOW=%d\n",
                rtc_clk_cpu_freq_value(rtc_clk_cpu_freq_get()),
                rtc_clk_apb_freq_get(), rtc_clk_xtal_freq_get()*MHZ,
                RTC_FAST_FREQ_8M_MHZ, rtc_clk_slow_freq_get_hz());
    LOG_STARTUP("XTAL calibration value: %d\n", esp_clk_slowclk_cal_get());
    LOG_STARTUP("Heap free: %u bytes\n", get_free_heap_size());
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    /* initialize stdio */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* print system time */
#ifdef MODULE_PERIPH_RTC
    struct tm _sys_time;
    rtc_get_time(&_sys_time);
    LOG_STARTUP("System time: %04d-%02d-%02d %02d:%02d:%02d\n",
                _sys_time.tm_year + 1900, _sys_time.tm_mon + 1, _sys_time.tm_mday,
                _sys_time.tm_hour, _sys_time.tm_min, _sys_time.tm_sec);
#endif

    /* print the board config */
#ifdef MODULE_ESP_LOG_STARTUP
    print_board_config();
#endif

    #if MODULE_MTD
    /* init flash drive */
    extern void spi_flash_drive_init (void);
    spi_flash_drive_init();
    #endif

    /* initialize the board */
    board_init();

    /* route a software interrupt source to CPU as trigger for thread yields */
    intr_matrix_set(PRO_CPU_NUM, ETS_FROM_CPU_INTR0_SOURCE, CPU_INUM_SOFTWARE);
    /* set thread yield handler and enable the software interrupt */
    xt_set_interrupt_handler(CPU_INUM_SOFTWARE, thread_yield_isr, NULL);
    xt_ints_on(BIT(CPU_INUM_SOFTWARE));

    /* initialize ESP system event loop */
    extern void esp_event_handler_init(void);
    esp_event_handler_init();

    /* starting RIOT */
#ifdef MODULE_ESP_LOG_STARTUP
    LOG_STARTUP("Starting RIOT kernel on PRO cpu\n");
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
#else
    puts("");
#endif
    kernel_init();
    UNREACHABLE();
}

static void intr_matrix_clear(void)
{
    /* attach all peripheral interrupt sources (Technical Reference, Table 7) */
    /* to an arbitrary CPU interrupt number (Technical Reference, Table 8) */
    for (int i = ETS_WIFI_MAC_INTR_SOURCE; i <= ETS_CACHE_IA_INTR_SOURCE; i++) {
        intr_matrix_set(PRO_CPU_NUM, i, ETS_INVALID_INUM);
    }
}
