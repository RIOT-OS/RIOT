/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble
 * @{
 *
 * @file
 * @brief       Initialization of the nimble controller
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "thread.h"
#include "nimble_riot.h"

#include "nimble/nimble_port.h"

#ifdef CPU_FAM_NRF52
#include "nrf_clock.h"
#endif

static char stack[THREAD_STACKSIZE_DEFAULT];

void nimble_riot_controller_init(void)
{
#ifdef CPU_FAM_NRF52
    clock_start_lf();
#endif

    /*
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority.
     */
    thread_create(stack, sizeof(stack), NIMBLE_CONTROLLER_PRIO, 0,
                  (thread_task_func_t)nimble_port_ll_task_func,
                  NULL, "nimble_ctrl");
}
