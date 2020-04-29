/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_pm_layered
 * @{
 *
 * @file
 * @brief       Platform-independent power management code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "pm_layered.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_NUM_MODES
#error PM_NUM_MODES must be defined in periph_cpu.h!
#endif

#ifndef PM_BLOCKER_INITIAL
#define PM_BLOCKER_INITIAL 0x01010101   /* block all by default */
#endif

/**
 * @brief Global variable for keeping track of blocked modes
 */
static pm_blocker_t pm_blocker = { .val_u32 = PM_BLOCKER_INITIAL };

void pm_set_lowest(void)
{
    unsigned state = irq_disable();
    unsigned mode = PM_NUM_MODES;
    while (mode) {
        if (pm_blocker.val_u8[mode-1]) {
            break;
        }
        mode--;
    }

    pm_set(mode);
    irq_restore(state);
}

void pm_block(unsigned mode)
{
    unsigned state = irq_disable();
    assert(pm_blocker.val_u8[mode] != 255);
    pm_blocker.val_u8[mode]++;
    irq_restore(state);
}

void pm_unblock(unsigned mode)
{
    unsigned state = irq_disable();
    assert(pm_blocker.val_u8[mode] > 0);
    pm_blocker.val_u8[mode]--;
    irq_restore(state);
}

pm_blocker_t pm_get_blocker(void)
{
    pm_blocker_t result;
    unsigned state = irq_disable();
    result = pm_blocker;
    irq_restore(state);
    return result;
}

#ifndef PROVIDES_PM_LAYERED_OFF
void pm_off(void)
{
    irq_disable();
    pm_blocker.val_u32 = 0;
    pm_set_lowest();
    while(1) {}
}
#endif
