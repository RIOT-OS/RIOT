/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file        wdt.c
 * @brief       Low-level WDT driver implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include "periph/wdt.h"
#include "pm_layered.h"
#include "board.h"

#include "debug.h"

#ifndef WDT_CLOCK_HZ
#define WDT_CLOCK_HZ 1024
#endif

/* work around inconsistency in header files */
#ifndef WDT_CONFIG_PER_8_Val
#define WDT_CONFIG_PER_8_Val WDT_CONFIG_PER_CYC8_Val
#endif
#ifndef WDT_CONFIG_PER_8K_Val
#define WDT_CONFIG_PER_8K_Val WDT_CONFIG_PER_CYC8192_Val
#endif
#ifndef WDT_CONFIG_PER_16K_Val
#define WDT_CONFIG_PER_16K_Val WDT_CONFIG_PER_CYC16384_Val
#endif

static inline void _wdt_enable(void)
{
/* work around strange watchdog behaviour if IDLE2 is used on samd21 */
#ifdef CPU_COMMON_SAMD21
        pm_block(1);
#endif

#ifdef WDT_CTRLA_ENABLE
    WDT->CTRLA.reg |= WDT_CTRLA_ENABLE;
#else
    WDT->CTRL.reg |= WDT_CTRL_ENABLE;
#endif
}

static inline void _wdt_disable(void)
{
#ifdef WDT_CTRLA_ENABLE
    WDT->CTRLA.reg &= ~WDT_CTRLA_ENABLE;
#else
    WDT->CTRL.reg &= ~WDT_CTRL_ENABLE;
#endif
}

static inline void _wait_syncbusy(void)
{
#ifdef WDT_STATUS_SYNCBUSY
    while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY) {}
#else
    while (WDT->SYNCBUSY.reg) {}
#endif
}

static uint32_t ms_to_per(uint32_t ms)
{
    const uint32_t cycles = (ms * WDT_CLOCK_HZ) / 1024;

    /* Minimum WDT period is 8 clock cycles (register value 0) */
    if (cycles <= 8) {
        return 0;
    }

    /* Round up to next pow2 and calculate the register value */
    return 29 - __builtin_clz(cycles - 1);
}

#ifdef CPU_COMMON_SAMD21
static void _wdt_clock_setup(void)
{
    /* Connect to GCLK3 (~1.024 kHz) */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT
                      | GCLK_CLKCTRL_GEN(SAM0_GCLK_1KHZ)
                      | GCLK_CLKCTRL_CLKEN;
}
#else
static void _wdt_clock_setup(void)
{
    /* nothing to do here */
}
#endif

void wdt_init(void)
{
    _wdt_clock_setup();
#ifdef MCLK
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_WDT;
#else
    PM->APBAMASK.reg |= PM_APBAMASK_WDT;
#endif

    _wdt_disable();
    NVIC_EnableIRQ(WDT_IRQn);
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    uint32_t per, win;

    if (max_time == 0) {
        DEBUG("invalid period: max_time = %"PRIu32"\n", max_time);
        return;
    }

    per = ms_to_per(max_time);

    if (per > WDT_CONFIG_PER_16K_Val) {
        DEBUG("invalid period: max_time = %"PRIu32"\n", max_time);
        return;
    }

    if (min_time) {
        win = ms_to_per(min_time);

        if (win > WDT_CONFIG_PER_8K_Val) {
            DEBUG("invalid period: min_time = %"PRIu32"\n", min_time);
            return;
        }

        if (per < win) {
            per = win + 1;
        }

#ifdef WDT_CTRLA_WEN
        WDT->CTRLA.reg |= WDT_CTRLA_WEN;
#else
        WDT->CTRL.reg |= WDT_CTRL_WEN;
#endif
    } else {
        win = 0;
#ifdef WDT_CTRLA_WEN
        WDT->CTRLA.reg &= ~WDT_CTRLA_WEN;
#else
        WDT->CTRL.reg &= ~WDT_CTRL_WEN;
#endif
    }

    WDT->INTFLAG.reg = WDT_INTFLAG_EW;

    DEBUG("watchdog window: %"PRIu32", period: %"PRIu32"\n", win, per);

    WDT->CONFIG.reg = WDT_CONFIG_WINDOW(win) | WDT_CONFIG_PER(per);
    _wait_syncbusy();
}

void wdt_stop(void)
{
    _wdt_disable();
    _wait_syncbusy();
}

void wdt_start(void)
{
    _wdt_enable();
    _wait_syncbusy();
}

void wdt_kick(void)
{
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY_Val;
}

#ifdef MODULE_PERIPH_WDT_CB
static wdt_cb_t cb;
static void* cb_arg;

void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t wdt_cb, void *arg)
{
    uint32_t per = ms_to_per(max_time);

    if (per == WDT_CONFIG_PER_8_Val && wdt_cb) {
        DEBUG("period too short for early warning\n");
        return;
    }

    cb = wdt_cb;
    cb_arg = arg;

    if (cb != NULL) {
        uint32_t warning_offset = ms_to_per(CONFIG_WDT_WARNING_PERIOD);

        if (warning_offset == 0) {
            warning_offset = 1;
        }

        if (warning_offset >= per) {
            warning_offset = per - 1;
        }

        WDT->INTENSET.reg = WDT_INTENSET_EW;
        uint32_t reg = WDT->EWCTRL.reg;
        WDT->EWCTRL.reg = (reg & ~WDT_EWCTRL_EWOFFSET_Msk)
                        |  WDT_EWCTRL_EWOFFSET(per - warning_offset);
    } else {
        WDT->INTENCLR.reg = WDT_INTENCLR_EW;
    }

    wdt_setup_reboot(min_time, max_time);
}

void isr_wdt(void)
{
    WDT->INTFLAG.reg = WDT_INTFLAG_EW;

    if (cb != NULL) {
        cb(cb_arg);
    }

    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_WDT_CB */
