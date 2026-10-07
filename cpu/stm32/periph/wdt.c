/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file        wdt.c
 * @brief       Independent Watchdog timer for STM32 platforms
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "cpu.h"
#include "timex.h"

#include "periph_cpu.h"
#include "periph/wdt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef CPU_FAM_STM32H7
/* use watchdog 1 for the H7 */
#  define IWDG IWDG1
#endif

#define MAX_RELOAD                (4096U)
#define MAX_PRESCALER             (6U)

#define IWDG_KR_KEY_RELOAD        ((uint16_t)0xAAAA)
#define IWDG_KR_KEY_ENABLE        ((uint16_t)0xCCCC)

#define IWDG_UNLOCK               ((uint16_t)0x5555)
#define IWDG_LOCK                 ((uint16_t)0x0000)

/* The prescaler and reload values are calculated by wdt_setup_reboot()
 * and have to be applied by wdt_start(), so they have to be stored.
 * Initialized with safe default values. */
static uint8_t _prescaler = 0;
static uint16_t _reload = IWDG_RLR_RL;
static bool _started;

static inline uint32_t _wdt_time(uint8_t pre, uint16_t rel)
{
    /* wdt_time (us) = LSI(us) x 4 x 2^PRE x RELOAD */
    return (uint32_t)(((uint64_t)US_PER_SEC * 4 * (1 << pre) * rel ) / CLOCK_LSI);
}

static void _set_config(void)
{
    assert(_prescaler <= MAX_PRESCALER);
    assert(_reload <= IWDG_RLR_RL);

    /* PR and RLR can't be written while an update is ongoing. Updates only
     * complete while the watchdog is running, so this must not be called
     * before starting it. */
    while (IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) {}

    IWDG->KR = IWDG_UNLOCK;
    IWDG->PR = _prescaler;
    IWDG->RLR = _reload;
    IWDG->KR = IWDG_LOCK;

    /* Wait for the update before reloading the counter with the new value */
    while (IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) {}
    wdt_kick();
}

static inline uint32_t _wdt_ticks(uint8_t pre, uint32_t rst_time)
{
    /* ticks = rst_time(ms) x LSI(kHz) / (4 x 2^PRE) */
    return (rst_time * (CLOCK_LSI / MS_PER_SEC)) / (4U << pre);
}

static uint8_t _find_prescaler(uint32_t rst_time)
{
    /* Find the smallest prescaler for which the ticks fit the reload value */
    uint8_t pre = 0;
    while ((pre < MAX_PRESCALER) && (_wdt_ticks(pre, rst_time) > MAX_RELOAD)) {
        pre++;
    }
    DEBUG("[wdt]: prescaler value %d\n", pre);
    return pre;
}

static uint16_t _find_reload_value(uint8_t pre, uint32_t rst_time)
{
    /* The watchdog expires after RELOAD + 1 ticks */
    uint16_t rel = (uint16_t)(_wdt_ticks(pre, rst_time) - 1);
    DEBUG("[wdt]: reload value %d\n", rel);
    return rel;
}

void wdt_start(void)
{
    IWDG->KR = IWDG_KR_KEY_ENABLE;
    _started = true;
    _set_config();
}

void wdt_kick(void)
{
    IWDG->KR = IWDG_KR_KEY_RELOAD;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;
    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time >= NWDT_TIME_LOWER_LIMIT) && (max_time <= NWDT_TIME_UPPER_LIMIT));

    _prescaler = _find_prescaler(max_time);
    _reload = _find_reload_value(_prescaler, max_time);

    DEBUG("[wdt]: reset time %" PRIu32 " [us]\n", _wdt_time(_prescaler, _reload + 1));

    /* If the WDT is not already running, the configuration will be applied
     * when starting the watchdog. */
    if (_started) {
        _set_config();
    }
}
