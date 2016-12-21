/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Platform-independent power management code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_PM

volatile pm_blocker_t pm_blocker = PM_BLOCKER_INITIAL;

void pm_set_lowest(void)
{
    pm_blocker_t blocker = (pm_blocker_t) pm_blocker;
    unsigned mode = 0;
    while (mode < PM_NUM_MODES) {
        if (! (blocker.val_u8[mode])) {
            break;
        }
        mode++;
    }

    /* set lowest mode if blocker is still the same */
    unsigned state = irq_disable();
    if (blocker.val_u32 == pm_blocker.val_u32) {
        DEBUG("pm: setting mode %u\n", mode);
        pm_set(mode);
    }
    else {
        DEBUG("pm: mode block changed\n");
    }
    irq_restore(state);
}

#endif /* MODULE_PM */
