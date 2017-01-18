/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            break;
    }

    /* Executes a device DSB (Data Synchronization Barrier) */
    __DSB();
    /* Enter standby mode */
    __WFI();
}
