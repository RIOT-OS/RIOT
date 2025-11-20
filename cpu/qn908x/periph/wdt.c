/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     cpu_qn908x_wdt
 *
 * @{
 *
 * @file
 * @brief       Low-level WDOG driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "periph/wdt.h"
#include <stdint.h>
#include <stdbool.h>

#include "vendor/drivers/fsl_clock.h"

#include "debug.h"

/* The number of cycles to refresh the WDT with when kicked. */
static uint32_t wdt_load_value = 0xffffffff;

/* The maximum value the WDT counter could have when kicked. The WDT counter
 * always decrements starting from wdt_load_value, so when used in WINDOW mode
 * a counter value larger than this means that WDT was kicked before the lower
 * bound of the window.
 */
static uint32_t wdt_load_window_value = 0xffffffff;

/* The value that will be loaded in the WDT counter after the first interrupt
 * triggers. The WDT doesn't not reset the device automatically once the WDT
 * counter reaches 0, instead it first triggers an interrupt and restarts the
 * count again. This value will be loaded by the ISR after it triggers. A value
 * of 0 means to reset immediately after the ISR triggers, which is used if no
 * callback is provided. */
static uint32_t wdt_load_after_isr = 0;

#ifdef MODULE_PERIPH_WDT_CB
static wdt_cb_t wdt_cb;
static void *wdt_arg;
static bool wdt_kick_disabled = false;
#endif /* MODULE_PERIPH_WDT_CB */

/**
 * Before making any change to the WDT it is required to "unlock" it by writing
 * this value to the LOCK register. Call @ref _wdt_lock() to lock it again.
 */
static void _wdt_unlock(void)
{
    WDT->LOCK = 0x1acce551;
}

/**
 * @brief Lock the WDT block to prevent accidental changes.
 */
static void _wdt_lock(void)
{
    WDT->LOCK = 0x10c1ced;  /* Any other value is as good. */
}

void wdt_start(void)
{
    CLOCK_EnableClock(kCLOCK_Wdt);
}

void wdt_stop(void)
{
    /* This only stops the clock of the watchdog and therefore the counter
     * stops, but leaves it otherwise configured. */
    CLOCK_DisableClock(kCLOCK_Wdt);
}

void wdt_kick(void)
{
#ifdef MODULE_PERIPH_WDT_CB
    if (wdt_kick_disabled) {
        return;
    }
#endif /* MODULE_PERIPH_WDT_CB */
    if (WDT->VALUE > wdt_load_window_value) {
        DEBUG("wdt_kick() called before the minimum window time.");
        /* In this condition we simulate the WDT triggering immediately by
         * setting its LOAD value to 0. This will cause the ISR (and the
         * callback if there is one) if it wasn't called yet, which will update
         * the LOAD value with the time derived from CONFIG_WDT_WARNING_PERIOD
         * if needed and reset the device afterwards. */
        _wdt_unlock();
        WDT->LOAD = 0;
        _wdt_lock();
        while (true) {}
    }
    _wdt_unlock();
    WDT->LOAD = wdt_load_value;
    _wdt_lock();
}

static void wdt_setup(uint32_t min_time, uint32_t max_time, uint32_t isr_time)
{
    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT)
           && (max_time < NWDT_TIME_UPPER_LIMIT));
    assert(min_time <= max_time);

    /* The clock is stopped after setup and calling wdt_start() is required. */
    wdt_stop();

    const uint32_t tick_per_ms = CLOCK_GetFreq(kCLOCK_WdtClk) / 1000;
    wdt_load_value = tick_per_ms * max_time;
    wdt_load_window_value = tick_per_ms * (max_time - min_time);
    wdt_load_after_isr = tick_per_ms * isr_time;
    /* The ISR is always called as it is needed to trigger the reset after the
     * appropriate delay even in the no callback case. */
    _wdt_unlock();
    WDT->LOAD = wdt_load_value;
    WDT->CTRL = WDT_CTRL_RESEN_MASK | WDT_CTRL_INTEN_MASK;
    _wdt_lock();
    NVIC_EnableIRQ(WDT_IRQn);
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
#ifdef MODULE_PERIPH_WDT_CB
    wdt_cb = NULL;
    wdt_arg = NULL;
#endif /* MODULE_PERIPH_WDT_CB */
    wdt_setup(min_time, max_time, 0);
}

#ifdef MODULE_PERIPH_WDT_CB
void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t cb, void *arg)
{
    wdt_cb = cb;
    wdt_arg = arg;
    assert(max_time >= CONFIG_WDT_WARNING_PERIOD);
    /* We don't support having a min_time that falls within the
     * CONFIG_WDT_WARNING_PERIOD since that would mean that you can't call
     * wdt_kick() until some time after the callback is called which is pretty
     * useless considering that the purpose of the callback is to perform
     * "specific safety operations of data logging before the actual reboot."
     * before the reboot happens. After the callback is called the reboot is
     * inevitable and calling wdt_kick() has no effect. This code moves the
     * min_time back to at least the point where the callback is called which is
     * a similar behavior. */
    if (max_time - CONFIG_WDT_WARNING_PERIOD < min_time) {
        min_time = max_time - CONFIG_WDT_WARNING_PERIOD;
    }
    wdt_setup(min_time, max_time - CONFIG_WDT_WARNING_PERIOD,
              CONFIG_WDT_WARNING_PERIOD);
}
#endif /* MODULE_PERIPH_WDT_CB */

void isr_wdt(void)
{
    DEBUG("[wdt] isr_wdt with LOAD=%" PRIu32 "\n", WDT->LOAD);

    /* Set the timer to reset the device after CONFIG_WDT_WARNING_PERIOD but not
     * clear the interrupt bit in the WDT->INT_CLR register. This will cause the
     * WDT to reset the device the next time the counter reaches 0, which now
     * will happen again in CONFIG_WDT_WARNING_PERIOD ms.
     * Since the wdt_cb may return before the new WDT counter triggers, which is
     * the normal case if we expect the callback to do some short safety
     * operations, we would exit the ISR without clearing the interrupt bit in
     * WDT->INT_CLR which would cause the interrupt to be called again. To avoid
     * this situation, we disable the IRQ source in the NVIC.
     * After this ISR triggers, further wdt_kicks() are ignored to prevent the
     * software from kicking the WDT during the CONFIG_WDT_WARNING_PERIOD. */
    NVIC_DisableIRQ(WDT_IRQn);
    _wdt_unlock();
    WDT->LOAD = wdt_load_after_isr;
    _wdt_lock();

#ifdef MODULE_PERIPH_WDT_CB
    wdt_kick_disabled = true;
    if (wdt_cb) {
        wdt_cb(wdt_arg);
    }
#endif /* MODULE_PERIPH_WDT_CB */
    cortexm_isr_end();
}
