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
#include "host/ble_hs.h"
#include "host/util/util.h"

#ifdef MODULE_NIMBLE_SVC_GAP
#include "services/gap/ble_svc_gap.h"
#endif
#ifdef MODULE_NIMBLE_SVC_GATT
#include "services/gatt/ble_svc_gatt.h"
#endif
#ifdef MODULE_NIMBLE_SVC_IPSS
#include "services/ipss/ble_svc_ipss.h"
#endif

#ifdef MODULE_NIMBLE_STATCONN
#include "nimble_statconn.h"
#endif

#if defined(MODULE_NIMBLE_AUTOCONN) && !defined(MODULE_NIMBLE_AUTOCONN_NOAUTOINIT)
#include "nimble_autoconn.h"
#include "nimble_autoconn_params.h"
#endif

#ifdef MODULE_NIMBLE_CONTROLLER
#if defined(CPU_FAM_NRF52) || defined(CPU_FAM_NRF51)
#include "nrf_clock.h"
#endif

static char _stack_controller[NIMBLE_CONTROLLER_STACKSIZE];
#endif

#ifdef MODULE_NIMBLE_HOST
static char _stack_host[NIMBLE_HOST_STACKSIZE];

uint8_t nimble_riot_own_addr_type;

static void *_host_thread(void *arg)
{
    (void)arg;

    nimble_port_init();

#ifdef MODULE_NIMBLE_CONTROLLER
    /* XXX: NimBLE needs the nRF5x's LF clock to run */
#if defined(CPU_FAM_NRF52) || defined(CPU_FAM_NRF51)
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
#endif

    nimble_port_run();

    /* never reached */
    return NULL;
}
#endif

void nimble_riot_init(void)
{
    int res;
    (void)res;

    /* and finally initialize and run the host */
    thread_create(_stack_host, sizeof(_stack_host),
                  NIMBLE_HOST_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _host_thread, NULL,
                  "nimble_host");

    /* make sure synchronization of host and controller is done, this should
     * always be the case at this point */
    while (!ble_hs_synced()) {}

    /* for reducing code duplication, we read our own address type once here
     * so it can be re-used later on */
    res = ble_hs_util_ensure_addr(0);
    assert(res == 0);
    res = ble_hs_id_infer_auto(0, &nimble_riot_own_addr_type);
    assert(res == 0);

#ifdef MODULE_NIMBLE_NETIF
    extern void nimble_netif_init(void);
    nimble_netif_init();
#ifdef MODULE_SHELL_COMMANDS
    extern void sc_nimble_netif_init(void);
    sc_nimble_netif_init();
#endif
#endif

    /* initialize the configured, built-in services */
#ifdef MODULE_NIMBLE_SVC_GAP
    ble_svc_gap_init();
#endif
#ifdef MODULE_NIMBLE_SVC_GATT
    ble_svc_gatt_init();
#endif
#ifdef MODULE_NIMBLE_SVC_IPSS
    ble_svc_ipss_init();
#endif

#ifdef MODULE_NIMBLE_STATCONN
    nimble_statconn_init();
#endif

#if defined(MODULE_NIMBLE_AUTOCONN) && !defined(MODULE_NIMBLE_AUTOCONN_NOAUTOINIT)
    ble_gatts_start();
    /* CAUTION: this must be called after nimble_netif_init() and also only
     *          after the GATT server has been initialized */
    res = nimble_autoconn_init(&nimble_autoconn_params, NULL, 0);
    assert(res == NIMBLE_AUTOCONN_OK);
    nimble_autoconn_enable();
#endif
}
