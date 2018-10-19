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
 * @brief       Glue code for running NimBLE for RIOT
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "thread.h"
#include "nimble_riot.h"

#include "nimble/nimble_port.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#ifdef CPU_FAM_NRF52
#include "nrf_clock.h"
#endif

static char _stack_controller[NIMBLE_CONTROLLER_STACKSIZE];
static char _stack_host[NIMBLE_HOST_STACKSIZE];

static void *_host_thread(void *arg)
{
    (void)arg;

    nimble_port_init();
    nimble_port_run();

    /* never reached */
    return NULL;
}

void nimble_riot_init(void)
{
    /* XXX: NimBLE needs the nRF5x's LF clock to run */
#ifdef CPU_FAM_NRF52
    clock_start_lf();
#endif

    /* Run the controller
     *
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority.
     */
    thread_create(_stack_controller, sizeof(_stack_controller),
                  NIMBLE_CONTROLLER_PRIO,
                  THREAD_CREATE_STACKTEST,
                  (thread_task_func_t)nimble_port_ll_task_func, NULL,
                  "nimble_ctrl");

    /* and finally initialize and run the host */
    thread_create(_stack_host, sizeof(_stack_host),
                  NIMBLE_HOST_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _host_thread, NULL,
                  "nimble_host");
}
