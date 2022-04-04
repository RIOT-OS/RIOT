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

#include <assert.h>
#include <string.h>

#include "board.h"
#include "irq.h"
#include "periph/pm.h"
#include "pm_layered.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PM_NUM_MODES
#error PM_NUM_MODES must be defined in periph_cpu.h!
#endif

#ifndef PM_BLOCKER_INITIAL
#if PM_NUM_MODES == 1
#define PM_BLOCKER_INITIAL { 0 }
#endif
#if PM_NUM_MODES == 2
#define PM_BLOCKER_INITIAL { 1, 0 }
#endif
#if PM_NUM_MODES == 3
#define PM_BLOCKER_INITIAL { 1, 1, 0 }
#endif
#if PM_NUM_MODES == 4
#define PM_BLOCKER_INITIAL { 1, 1, 1, 0 }
#endif
#if PM_NUM_MODES == 5
#define PM_BLOCKER_INITIAL { 1, 1, 1, 1, 0 }
#endif
#endif

/**
 * @brief Global variable for keeping track of blocked modes
 */
static pm_blocker_t pm_blocker = { .blockers = PM_BLOCKER_INITIAL };

void pm_set_lowest(void)
{
    unsigned mode = PM_NUM_MODES;

    /* set lowest mode if blocker is still the same */
    unsigned state = irq_disable();
    while (mode) {
        if (pm_blocker.blockers[mode - 1]) {
            break;
        }
        mode--;
    }

    if (mode != PM_NUM_MODES) {
        pm_set(mode);
    }
    irq_restore(state);
}

void pm_block(unsigned mode)
{
    assert(pm_blocker.blockers[mode] != 255);

    unsigned state = irq_disable();
    pm_blocker.blockers[mode]++;
    irq_restore(state);
}

void pm_unblock(unsigned mode)
{
    assert(pm_blocker.blockers[mode] > 0);

    unsigned state = irq_disable();
    pm_blocker.blockers[mode]--;
    irq_restore(state);
}

pm_blocker_t pm_get_blocker(void)
{
    pm_blocker_t result;

    unsigned state = irq_disable();
    memcpy(&result, &pm_blocker, sizeof(result));
    irq_restore(state);

    return result;
}

#ifndef PROVIDES_PM_LAYERED_OFF
void pm_off(void)
{
    irq_disable();
    while(1) {
        pm_set(0);
    }
}
#endif
