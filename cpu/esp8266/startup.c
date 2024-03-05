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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "kernel_init.h"
#include "periph/init.h"
#include "periph/uart.h"

#include "board.h"

#include "esp/common_macros.h"
#include "esp_log.h"
#include "esp_system.h"
#include "exceptions.h"
#include "stdio_base.h"
#include "syscalls.h"
#include "thread_arch.h"

#include "rom_functions.h"
#include "sdk/sdk.h"

#if MODULE_ESP_GDBSTUB
#include "gdbstub.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/* external esp function declarations */
extern uint32_t hwrand (void);

void esp_riot_init(void)
{
    /* clear RTC bss data */
    extern uint8_t _rtc_bss_start, _rtc_bss_end;
    esp_reset_reason_t reset_reason = esp_reset_reason();
    if (reset_reason != ESP_RST_DEEPSLEEP && reset_reason != ESP_RST_SW) {
        /* cppcheck-suppress comparePointers */
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start));
    }

    /* enable cached read from flash */
    Cache_Read_Enable_New();

    /* initialize the ISR stack for usage measurements */
    thread_isr_stack_init();

#ifndef CPU_ESP8266
    /* initialize newlib system calls */
    syscalls_init ();
#endif

    /* set system frequency if not 80 MHz */
    if (ESP8266_CPU_FREQUENCY != 80) {
        system_update_cpu_freq(ESP8266_CPU_FREQUENCY);
    }

    ets_printf("\n");
#ifdef MODULE_ESP_LOG_STARTUP
    ets_printf("Starting ESP8266 CPU with ID: %08x\n", system_get_chip_id());
    ets_printf("ESP8266-RTOS-SDK Version %s\n\n", system_get_sdk_version());
    ets_printf("CPU clock frequency: %d MHz\n", system_get_cpu_freq());
    extern void heap_stats(void);
    heap_stats();
#endif

    /* set exception handlers */
    init_exceptions ();

    /* systemwide UART initialization */
    extern void uart_system_init (void);
    uart_system_init();

    /* init watchdogs */
    system_wdt_init();

    /* init random number generator */
    srand(hwrand());

#if MODULE_MTD
    /* init flash drive */
    extern void spi_flash_drive_init (void);
    spi_flash_drive_init();
#endif

    /* initialize stdio*/
    extern int stdio_is_initialized;
    early_init();
    stdio_is_initialized = 1;

    /* trigger static peripheral initialization */
    periph_init();

    /* trigger board initialization */
    board_init();

#ifdef MODULE_ESP_LOG_STARTUP
    /* print the board config */
    board_print_config();
#else
    /* to have an empty line after the unreadable characters from ROM loader */
    puts("");
#endif

    /* initialize ESP system event loop */
    extern void esp_event_handler_init(void);
    esp_event_handler_init();

    /* activate software interrupt based context switch */
    extern void IRAM thread_yield_isr(void* arg);
    ets_isr_attach(ETS_SOFT_INUM, thread_yield_isr, NULL);
    ets_isr_unmask(BIT(ETS_SOFT_INUM));

#ifdef MODULE_ESP_GDBSTUB
    gdbstub_init();
#endif
}

void esp_riot_start(void)
{
    /* does not return */
    kernel_init();
}

void __wrap_pp_attach(void)
{
#ifdef MODULE_ESP_WIFI_ANY
    extern void __real_pp_attach(void);
    __real_pp_attach();
#endif
}

void __wrap_pm_attach(void)
{
#ifdef MODULE_ESP_WIFI_ANY
    extern void __real_pm_attach(void);
    __real_pm_attach();
#endif
}
