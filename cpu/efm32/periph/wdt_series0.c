/*
 * SPDX-FileCopyrightText: 2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file
 * @brief       Watchdog timer peripheral driver implementation for
 *              EFM32 Series 0 MCUs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <stdlib.h>
#include <assert.h>

#include "cpu.h"

#include "periph/wdt.h"

#include "em_cmu.h"
#include "em_wdog.h"

#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static WDOG_PeriodSel_TypeDef _get_prescaler(uint32_t max_time)
{
    const uint32_t cycles = (max_time * WDT_CLOCK_HZ) / 1000;

    DEBUG("[wdt_series0] _get_prescaler: cycles=%" PRIu32 "\n", cycles);

    return (WDOG_PeriodSel_TypeDef) (32 - __builtin_clz(cycles - 1) - 3);
}

void wdt_kick(void)
{
    WDOGn_Feed(WDOG);
}

void wdt_start(void)
{
    WDOGn_Enable(WDOG, true);
}

void wdt_stop(void)
{
    WDOGn_Enable(WDOG, false);
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    /* avoid compilation errors when NDEBUG is defined */
    (void)min_time;
    (void)max_time;

    /* assert timings */
    assert(min_time == 0);
    assert(max_time > NWDT_TIME_LOWER_LIMIT ||
           max_time < NWDT_TIME_UPPER_LIMIT);

    /* initialize clock */
    CMU_ClockEnable(cmuClock_HFLE, true);

    /* initialize watchdog */
    WDOG_Init_TypeDef init = WDOG_INIT_DEFAULT;

    init.enable = false;
    init.clkSel = wdogClkSelULFRCO;
    init.perSel = _get_prescaler(max_time);

    DEBUG("[wdt_series0] wdt_setup_reboot: prescaler=%d calculated_time=%" PRIu32 "\n",
          (int) init.perSel,
          ((1 << (3 + (int)init.perSel)) + 1 ) / WDT_CLOCK_HZ * MS_PER_SEC);

    WDOGn_Init(WDOG, &init);
}
