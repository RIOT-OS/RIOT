/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rp2350
 * @{
 *
 * @file
 * @brief       Layered power management for the RP2350
 *
 * @author      Christopher Buechse
 * @}
 */

#include <stdbool.h>
#include <stdint.h>

#include "periph/pm.h"
#include "periph_cpu.h"

#ifdef RP2350_USE_RISCV
/** Gate the processor clock while asleep (Hazard3 msleep.deepsleep) */
#  define MSLEEP_DEEPSLEEP      (1u << 0)
/** Release the power-up request while asleep (Hazard3 msleep.powerdown) */
#  define MSLEEP_POWERDOWN      (1u << 1)
#endif

#ifdef MODULE_PM_LAYERED
void pm_set(unsigned mode)
{
    bool deep = (mode == RP2350_PM_SLEEP);

#ifdef RP2350_USE_RISCV
    uint32_t msleep = deep ? (MSLEEP_DEEPSLEEP | MSLEEP_POWERDOWN) : 0;

    /* pm_layered calls this with interrupts disabled, wfi still wakes up on
     * any pending interrupt that is enabled in mie. 0xbf0 is the custom
     * msleep CSR of Hazard3. */
    __asm__ volatile ("csrw 0xbf0, %0" : : "r" (msleep));
    __asm__ volatile ("wfi" : : : "memory");
    __asm__ volatile ("csrw 0xbf0, zero");
#else
    cortexm_sleep(deep);
#endif
}
#endif
