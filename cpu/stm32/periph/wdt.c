/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_wdt
 *
 * @brief
 *
 * @{
 *
 * @file        wdt.c
 * @brief       Independent Watchdog timer for STM32L platforms
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "cpu.h"
#include "timex.h"

#include "periph_cpu.h"
#include "periph/wdt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RELOAD                (4096U)
#define MAX_PRESCALER             (6U)
#define IWDG_STEP_MS              ((4U * US_PER_MS * MAX_RELOAD) / CLOCK_LSI)

#define IWDG_KR_KEY_RELOAD        ((uint16_t)0xAAAA)
#define IWDG_KR_KEY_ENABLE        ((uint16_t)0xCCCC)

#define IWDG_UNLOCK               ((uint16_t)0x5555)
#define IWDG_LOCK                 ((uint16_t)0x0000)

static inline uint32_t _wdt_time(uint8_t pre, uint16_t rel)
{
    /* wdt_time (us) = LSI(us) x 4 x 2^PRE x RELOAD */
    return (uint32_t)(((uint64_t) US_PER_SEC * 4 * (1 << pre) * rel ) / CLOCK_LSI);
}

static inline void _iwdt_unlock(void)
{
    IWDG->KR = IWDG_UNLOCK;
}

static inline void _iwdt_lock(void)
{
    IWDG->KR = IWDG_LOCK;
}

static void _set_prescaler(uint8_t prescaler)
{
    assert(prescaler <= MAX_PRESCALER);

    _iwdt_unlock();
    IWDG->PR = prescaler;
    _iwdt_lock();
}

static void _set_reload(uint16_t reload)
{
    assert(reload <= IWDG_RLR_RL);

    _iwdt_unlock();
    IWDG->RLR = reload;
    _iwdt_lock();
}

static uint8_t _find_prescaler(uint32_t rst_time)
{
    /* Divide by the range to get power of 2 of the prescaler */
    uint8_t pre = 32U - __builtin_clz(rst_time / IWDG_STEP_MS);
    DEBUG("[wdt]: prescaler value %d\n", pre);
    return pre;
}

static uint16_t _find_reload_value(uint8_t pre, uint32_t rst_time)
{
    /* Calculate best reload value = rst_time / LSI(ms) x 4 x 2^PRE */
    uint16_t rel = (uint16_t)((rst_time * CLOCK_LSI) / \
                             ((uint32_t) (US_PER_MS * 4 * (1 << pre))));
    DEBUG("[wdt]: reload value %d\n", rel);
    return rel;
}

void wdt_start(void)
{
    IWDG->KR = IWDG_KR_KEY_ENABLE;
}

#ifdef CPU_FAM_STM32L4
void wdt_init(void)
{
    FLASH->OPTR |= ~(FLASH_OPTR_IWDG_STOP || FLASH_OPTR_IWDG_STDBY);
}
#endif

void wdt_kick(void)
{
    IWDG->KR = IWDG_KR_KEY_RELOAD;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    (void) min_time;
    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) || \
           (max_time < NWDT_TIME_UPPER_LIMIT));

    uint8_t pre = _find_prescaler(max_time);
    uint16_t rel = _find_reload_value(pre, max_time);

    /* Set watchdog prescaler and reload value */
    _set_prescaler(pre);
    _set_reload(rel);

    DEBUG("[wdt]: reset time %" PRIu32 " [us]\n", _wdt_time(pre, rel));

    /* Refresh wdt counter */
    wdt_kick();
}

#ifdef __cplusplus
}
#endif
