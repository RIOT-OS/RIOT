/*
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file
 * @brief       Watchdog timer peripheral driver implementation for
 *              EFM32 Series 1 MCUs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Juergen Fitschen <me@jue.yt>
 * @}
 */

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "cpu.h"

#include "periph/pm.h"
#include "periph/wdt.h"

#include "em_cmu.h"
#include "em_wdog.h"

#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_PERIPH_WDT_CB
static wdt_cb_t wdt_cb;
static void *wdt_arg;
#endif

static inline uint32_t _get_clock(void)
{
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
    return WDT_CLOCK_HZ;
#else
    return CMU_ClockFreqGet(cmuClock_WDOG0);
#endif
}

static uint32_t _get_calculated_time(WDOG_PeriodSel_TypeDef period)
{
    return ((1 << (3 + (int)period)) + 1) / _get_clock() * MS_PER_SEC;
}

static  WDOG_PeriodSel_TypeDef _get_period(uint32_t max_time)
{
    const uint32_t cycles = (max_time * _get_clock()) / MS_PER_SEC;

    DEBUG("[wdt_series1] _get_period: cycles=%" PRIu32 "\n", cycles);

    return (WDOG_PeriodSel_TypeDef) (32 - __builtin_clz(cycles - 1) - 3);
}

static WDOG_WinSel_TypeDef _get_illegal_window(uint32_t min_time, uint32_t calculated_time)
{
    if (min_time == 0) {
        return wdogIllegalWindowDisable;
    }

    uint32_t slice = calculated_time / 8;

    for (unsigned i = 1; i < 8; i++) {
        if (min_time < (i * slice)) {
            return (WDOG_WinSel_TypeDef) i;
        }
    }

    return wdogIllegalWindowDisable;
}

static void _init(uint32_t min_time, uint32_t max_time, bool warn)
{
    /* avoid compilation errors when NDEBUG is defined */
    (void)min_time;
    (void)max_time;
#ifndef MODULE_PERIPH_WDT_CB
    (void)warn;
#endif

    /* assert timings */
    if (min_time == 0) {
        assert(max_time > NWDT_TIME_LOWER_LIMIT ||
               max_time < NWDT_TIME_UPPER_LIMIT);
    }
    else {
        assert(max_time > WDT_TIME_LOWER_LIMIT ||
               max_time < WDT_TIME_UPPER_LIMIT);
    }

    /* initialize clock */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
    CMU_ClockEnable(cmuClock_HFLE, true);
#else
    CMU_ClockSelectSet(cmuClock_WDOG0CLK, cmuSelect_ULFRCO);
    CMU_ClockEnable(cmuClock_WDOG0, true);
#endif

    /* initialize watchdog */
    WDOG_Init_TypeDef init = WDOG_INIT_DEFAULT;

    init.enable = false;
#if defined(_WDOG_CFG_EM1RUN_MASK)
    init.em1Run = true;
#endif
    init.em2Run = true;
    init.em3Run = true;
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
    init.clkSel = wdogClkSelULFRCO;
#endif
    init.perSel = _get_period(max_time);

    uint32_t calculated_time = _get_calculated_time(init.perSel);

    init.winSel = _get_illegal_window(min_time, calculated_time);

    DEBUG("[wdt_series1] _init: prescaler=%d, winsel=%d, calculated=%" PRIu32 "\n",
          (int) init.perSel, (int) init.winSel, calculated_time);

#ifdef MODULE_PERIPH_WDT_CB
    if (warn) {
        init.warnSel = wdogWarnTime50pct;
    }
#endif

    WDOGn_Init(WDOG0, &init);

    /* Configure interrupts */
    WDOGn_IntEnable(WDOG0, WDOG_IEN_WIN);

#ifdef MODULE_PERIPH_WDT_CB
    if (warn) {
        WDOGn_IntEnable(WDOG0, WDOG_IEN_WARN);
    }
#endif

    NVIC_EnableIRQ(WDOG0_IRQn);
}

void wdt_kick(void)
{
    WDOGn_Feed(WDOG0);
}

void wdt_start(void)
{
    WDOGn_Enable(WDOG0, true);
}

void wdt_stop(void)
{
    WDOGn_Enable(WDOG0, false);
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    _init(min_time, max_time, false);
}

#if defined(MODULE_PERIPH_WDT_CB)
void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t cb, void *arg)
{
    wdt_cb = cb;
    wdt_arg = arg;

    _init(min_time, max_time, true);
}
#endif

void isr_wdog0(void)
{
    uint32_t flags = WDOGn_IntGet(WDOG0);

    if (flags & WDOG_IEN_WIN) {
        WDOGn_IntClear(WDOG0, WDOG_IEN_WIN);

        pm_reboot();
    }

#if defined(MODULE_PERIPH_WDT_CB)
    if (flags & WDOG_IEN_WARN) {
        WDOGn_IntClear(WDOG0, WDOG_IEN_WARN);

        if (wdt_cb) {
            wdt_cb(wdt_arg);
        }
    }
#endif

    cortexm_isr_end();
}
