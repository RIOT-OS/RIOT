/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of power management functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include <stdbool.h>

#include "irq.h"

#include "esp/xtensa_ops.h"
#include "sdk/ets_task.h"
#include "sdk/sdk.h"

#include "syscalls.h"

void pm_set_lowest(void)
{
    DEBUG ("%s\n", __func__);

    /* execute all pending system tasks before going to sleep */
    /* is it really necessary, the timer interrupt is thrown every some ms? */
    ets_tasks_run ();

    #if !defined(QEMU)
    DEBUG ("%s enter to sleep @%u\n", __func__, phy_get_mactime());

    /* passive wait for interrupt to leave lowest power mode */
    __asm__ volatile ("waiti 0");

    DEBUG ("%s exit from sleep @%u\n", __func__, phy_get_mactime());
    #endif

    /*
     * We could execute all pending system tasks after an interrupt before
     * continuing RIOT. However, to give RIOT tasks the highest priority,
     * *ets_tasks_run* should be called only before going to sleep
     */
    ets_tasks_run ();
}

void pm_off(void)
{
    DEBUG ("%s\n", __func__);
    system_deep_sleep(0);
}

void pm_reboot(void)
{
    DEBUG ("%s\n", __func__);

    /* shut down WIFI and call system_restart_local after timer */
    system_restart ();
}
